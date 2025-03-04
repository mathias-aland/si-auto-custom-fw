using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Media;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace si_auto_control
{
    public partial class Form1 : Form
    {

        modbus_ascii mb = new modbus_ascii();


        // modbus address map
        public const UInt16 MODBUS_COIL_NORMAL_CYCLE_STATE = 100;
        public const UInt16 MODBUS_COIL_DEGAS_CYCLE_STATE = 101;
        public const UInt16 MODBUS_COIL_DRAIN_CYCLE_STATE = 102;
        public const UInt16 MODBUS_COIL_HEATER_ENABLED = 103;
        public const UInt16 MODBUS_COIL_RECIRC_ENABLED = 104;
        public const UInt16 MODBUS_COIL_AIR_ENABLED = 105;
        public const UInt16 MODBUS_COIL_WLP_ENABLED = 106;
        public const UInt16 MODBUS_COIL_AUTODRAIN_ENABLED = 107;

        public const UInt16 MODBUS_HOLDING_SET_TIME = 2000;
        public const UInt16 MODBUS_HOLDING_DEGAS_TIME = 2001;
        public const UInt16 MODBUS_HOLDING_AUTODRAIN_TIME = 2002;
        public const UInt16 MODBUS_HOLDING_SET_TEMP = 2003;
        public const UInt16 MODBUS_HOLDING_DEGAS_PULSE_OFF = 2004;
        public const UInt16 MODBUS_HOLDING_DEGAS_PULSE_ON = 2005;
        public const UInt16 MODBUS_HOLDING_SET_OUTPUTS1 = 2100;
        public const UInt16 MODBUS_HOLDING_SET_OUTPUTS2 = 2101;

        public const UInt16 MODBUS_INPUT_ELAPSED_TIME = 20000;
        public const UInt16 MODBUS_INPUT_AUTODRAIN_TIMER = 20001;
        public const UInt16 MODBUS_INPUT_DEGAS_PULSE_TIMER = 20002;
        public const UInt16 MODBUS_INPUT_TEMP = 20003;
        public const UInt16 MODBUS_INPUT_INPUTSTATE = 20004;
        public const UInt16 MODBUS_INPUT_OUTPUTSTATE = 20005;


        bool normal_cycle_state = false;
        bool degas_cycle_state = false;
        bool drain_cycle_state = false;
        bool heater_enabled = false;
        bool recirc_enabled = false;
        bool air_enabled = false;







        class paramsClass
        {
            public bool wlp_enabled;
            public bool autodrain_enabled;

            public UInt16 set_time;
            public UInt16 degas_time;
            public UInt16 autodrain_time;
            public UInt16 set_temp;
            public UInt16 degas_pulse_off;
            public UInt16 degas_pulse_on;
        }

        paramsClass paramsLast = new paramsClass() {wlp_enabled = false, autodrain_enabled = false, autodrain_time = 0, degas_pulse_off = 0, degas_pulse_on = 0, degas_time = 0, set_temp = 0, set_time = 0 };
        paramsClass paramsRead = new paramsClass() { wlp_enabled = false, autodrain_enabled = false, autodrain_time = 0, degas_pulse_off = 0, degas_pulse_on = 0, degas_time = 0, set_temp = 0, set_time = 0 };

        UInt16 elapsed_time = 0;
        UInt16 autodrain_timer = 0;
        UInt16 degas_pulse_timer = 0;
        Byte temp1 = 0;
        Byte temp2 = 0;
        Byte input1_state = 0;
        Byte input2_state = 0;
        Byte output1_state = 0;
        Byte output2_state = 0;

        Stopwatch stopwatch = new Stopwatch();

        long maxElapsed = 0;
        long minElapsed = long.MaxValue;

        private static SemaphoreSlim comm_semaphore = new SemaphoreSlim(1,1);

        class readResult
        {
            public bool[] coils { get; set; }
            public byte[] holdingRegs { get; set; }

            public byte[] inputRegs { get; set; }
        }


        public Form1()
        {
            InitializeComponent();
        }

        private void cbComPort_SelectedIndexChanged(object sender, EventArgs e)
        {
            cbComPort.SelectedIndex = cbComPort.FindStringExact(mb.setPort(cbComPort.Text));
        }

        private void cbComPort_DropDown(object sender, EventArgs e)
        {
            string[] port = SerialPort.GetPortNames();

            string selPort = cbComPort.Text;

            cbComPort.Items.Clear();

            if (port.Length > 0)
            {
                Array.Sort(port);
                cbComPort.Items.AddRange(port);

                var idx = cbComPort.FindStringExact(selPort);

                if (idx != -1)
                {
                    cbComPort.SelectedIndex = idx;
                }

            }
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (tmrRead.Enabled)
            {
                tmrRead.Stop();
                btnConnect.Text = "Connect";
            }
            else
            {
                maxElapsed = 0;
                minElapsed = long.MaxValue;

                tmrRead.Start();
                btnConnect.Text = "Disconnect";
            }
        }

        private void bgReadData_DoWork(object sender, DoWorkEventArgs e)
        {

            comm_semaphore.Wait();

            stopwatch.Restart();

            try
            {
                mb.openPort();

                readResult res = new readResult();

                res.coils = mb.modbusASCII_readCoils(0, MODBUS_COIL_NORMAL_CYCLE_STATE, 8);
                res.holdingRegs = mb.modbusASCII_readHoldReg(0, MODBUS_HOLDING_SET_TIME, 6);
                res.inputRegs = mb.modbusASCII_readReg(0, MODBUS_INPUT_ELAPSED_TIME, 6);

                e.Result = res;
            }
            catch (Exception ex)
            {
                e.Result = null;
            }

            mb.closePort();

            stopwatch.Stop();

            comm_semaphore.Release();
        }

        private void bgReadData_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            //stopwatch.Stop();

            if (stopwatch.ElapsedMilliseconds < minElapsed)
                minElapsed = stopwatch.ElapsedMilliseconds;

            if (stopwatch.ElapsedMilliseconds > maxElapsed)
                maxElapsed = stopwatch.ElapsedMilliseconds;

            label1.Text = "Last readout duration: " + stopwatch.ElapsedMilliseconds.ToString() + " ms (min = " + minElapsed.ToString() + " ms, max = " + maxElapsed.ToString() + " ms)";

            if (e.Result == null)
            {
                lblState.Text = "COMM ERROR";
            }
            else
            {
                //textBox1.AppendText(" OK!" + System.Environment.NewLine);

                readResult res = (readResult)e.Result;





                normal_cycle_state = res.coils[0];
                degas_cycle_state = res.coils[1];
                drain_cycle_state = res.coils[2];
                heater_enabled = res.coils[3];
                recirc_enabled = res.coils[4];
                air_enabled = res.coils[5];
                paramsRead.wlp_enabled = res.coils[6];
                paramsRead.autodrain_enabled = res.coils[7];

                paramsRead.set_time = (UInt16)((res.holdingRegs[0] << 8) | (res.holdingRegs[1] & 0xff));
                paramsRead.degas_time = (UInt16)((res.holdingRegs[2] << 8) | (res.holdingRegs[3] & 0xff)); ;
                paramsRead.autodrain_time = (UInt16)((res.holdingRegs[4] << 8) | (res.holdingRegs[5] & 0xff)); ;
                paramsRead.set_temp = (UInt16)((res.holdingRegs[6] << 8) | (res.holdingRegs[7] & 0xff)); ;
                paramsRead.degas_pulse_off = (UInt16)((res.holdingRegs[8] << 8) | (res.holdingRegs[9] & 0xff)); ;
                paramsRead.degas_pulse_on = (UInt16)((res.holdingRegs[10] << 8) | (res.holdingRegs[11] & 0xff)); ;

                elapsed_time = (UInt16)((res.inputRegs[0] << 8) | (res.inputRegs[1] & 0xff));
                autodrain_timer = (UInt16)((res.inputRegs[2] << 8) | (res.inputRegs[3] & 0xff));
                degas_pulse_timer = (UInt16)((res.inputRegs[4] << 8) | (res.inputRegs[5] & 0xff));
                temp1 = res.inputRegs[6];
                temp2 = res.inputRegs[7];
                input1_state = res.inputRegs[8];
                input2_state = res.inputRegs[9];
                output1_state = res.inputRegs[10];
                output2_state = res.inputRegs[11];


                lblIn1State.Text = Convert.ToString(input1_state, 2).PadLeft(8,'0');
                lblIn2State.Text = Convert.ToString(input2_state, 2).PadLeft(8, '0');

                lblOut1State.Text = Convert.ToString(output1_state, 2).PadLeft(8, '0');
                lblOut2State.Text = Convert.ToString(output2_state, 2).PadLeft(8, '0');

                if (normal_cycle_state)
                {
                    lblState.Text = "RUN: " + TimeSpan.FromSeconds(elapsed_time).ToString(@"h\:mm\:ss");
                }
                else if (degas_cycle_state)
                {
                    lblState.Text = "DEGAS: " + TimeSpan.FromSeconds(elapsed_time).ToString(@"h\:mm\:ss");
                }
                else if (drain_cycle_state)
                {
                    if (paramsRead.autodrain_enabled)
                        lblState.Text = "AUTO DRAIN: " + TimeSpan.FromSeconds(autodrain_timer).ToString(@"m\:ss");
                    else
                        lblState.Text = "DRAIN";
                }
                else
                {
                    lblState.Text = "IDLE";
                }


                lblT1.Text = "T1: " + temp1.ToString() + " °C";
                lblT2.Text = "T2: " + temp2.ToString() + " °C";


                bool wlvl_over = (input1_state & (1 << 2)) == 0;
                bool wlvl_base = (input1_state & (1 << 1)) == 0;
                bool wlvl_top = (input1_state & (1 << 0)) == 0;


                string wlvlStr = "W. LEVEL: ";

                if (wlvl_base)
                    wlvlStr += "B ";
                else
                    wlvlStr += "  ";

                if (wlvl_top)
                    wlvlStr += "T ";
                else
                    wlvlStr += "  ";

                if (wlvl_over)
                    wlvlStr += "O";
                else
                    wlvlStr += " ";


                lblWLVL.Text = wlvlStr;

                string state2txt = String.Empty;


                if (heater_enabled)
                    state2txt += "HEAT  ";
                else
                    state2txt += "      ";

                if (recirc_enabled)
                    state2txt += "RECIRC  ";
                else
                    state2txt += "        ";

                if (air_enabled)
                    state2txt += "AIR";
                else
                    state2txt += "   ";

                lblState2.Text = state2txt;



                if (paramsRead.autodrain_enabled != paramsLast.autodrain_enabled)
                {
                    paramsLast.autodrain_enabled = paramsRead.autodrain_enabled;
                    cbAutoDrain.Checked = paramsRead.autodrain_enabled;
                }

                if (paramsRead.autodrain_time != paramsLast.autodrain_time)
                {
                    paramsLast.autodrain_time = paramsRead.autodrain_time;
                    numAutoDrainTime.Value = paramsRead.autodrain_time;
                }

                if (paramsRead.degas_pulse_off != paramsLast.degas_pulse_off)
                {
                    paramsLast.degas_pulse_off = paramsRead.degas_pulse_off;
                    numDegasOff.Value = paramsRead.degas_pulse_off;
                }

                if (paramsRead.degas_pulse_on != paramsLast.degas_pulse_on)
                {
                    paramsLast.degas_pulse_on = paramsRead.degas_pulse_on;
                    numDegasON.Value = paramsRead.degas_pulse_on;
                }

                if (paramsRead.degas_time != paramsLast.degas_time)
                {
                    paramsLast.degas_time = paramsRead.degas_time;
                    numDegasDuration.Value = paramsRead.degas_time;
                }

                if (paramsRead.set_temp != paramsLast.set_temp)
                {
                    paramsLast.set_temp = paramsRead.set_temp;
                    numSetTemp.Value = paramsRead.set_temp;
                }

                if (paramsRead.set_time != paramsLast.set_time)
                {
                    paramsLast.set_time = paramsRead.set_time;
                    numCycleTimeH.Value = TimeSpan.FromMinutes(paramsLast.set_time).Hours;
                    numCycleTimeM.Value = TimeSpan.FromMinutes(paramsLast.set_time).Minutes;
                }

                if (paramsRead.wlp_enabled != paramsLast.wlp_enabled)
                {
                    paramsLast.wlp_enabled = paramsRead.wlp_enabled;
                    cbWLP.Checked = paramsRead.wlp_enabled;
                }

            }

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] port = SerialPort.GetPortNames();

            cbComPort.Items.Clear();

            if (port.Length > 0)
            {
                Array.Sort(port);
                cbComPort.Items.AddRange(port);
                cbComPort.SelectedIndex = 0;
            }
        }

        private void tmrRead_Tick(object sender, EventArgs e)
        {

            if (!bgReadData.IsBusy)
            {
                //stopwatch.Restart();
                bgReadData.RunWorkerAsync();
            }
                
        }

        private void bgSetParams_DoWork(object sender, DoWorkEventArgs e)
        {
            comm_semaphore.Wait();

            try
            {
                mb.openPort();

                paramsClass paramsSet = (paramsClass)e.Argument;

                byte[] mbBytes = new byte[12];

                mbBytes[0] = (byte)(paramsSet.set_time >> 8);           // MSB
                mbBytes[1] = (byte)(paramsSet.set_time & 0xFF);         // LSB
                mbBytes[2] = (byte)(paramsSet.degas_time >> 8);         // MSB
                mbBytes[3] = (byte)(paramsSet.degas_time & 0xFF);       // LSB
                mbBytes[4] = (byte)(paramsSet.autodrain_time >> 8);     // MSB
                mbBytes[5] = (byte)(paramsSet.autodrain_time & 0xFF);   // LSB
                mbBytes[6] = (byte)(paramsSet.set_temp >> 8);           // MSB
                mbBytes[7] = (byte)(paramsSet.set_temp & 0xFF);         // LSB
                mbBytes[8] = (byte)(paramsSet.degas_pulse_off >> 8);    // MSB
                mbBytes[9] = (byte)(paramsSet.degas_pulse_off & 0xFF);  // LSB
                mbBytes[10] = (byte)(paramsSet.degas_pulse_on >> 8);    // MSB
                mbBytes[11] = (byte)(paramsSet.degas_pulse_on & 0xFF);  // LSB

                if (!mb.modbusASCII_writeHoldRegs(0, MODBUS_HOLDING_SET_TIME, 6, mbBytes))
                {
                    throw new InvalidOperationException();
                }

                if (!mb.modbusASCII_writeCoil(0, MODBUS_COIL_WLP_ENABLED, paramsSet.wlp_enabled))
                {
                    throw new InvalidOperationException();
                }

                if (!mb.modbusASCII_writeCoil(0, MODBUS_COIL_AUTODRAIN_ENABLED, paramsSet.autodrain_enabled))
                {
                    throw new InvalidOperationException();
                }

                e.Result = true;

                //readResult res = new readResult();

                //res.coils = mb.modbusASCII_readCoils(0, MODBUS_COIL_NORMAL_CYCLE_STATE, 8);
                //res.holdingRegs = mb.modbusASCII_readHoldReg(0, MODBUS_HOLDING_SET_TIME, 6);
                //res.inputRegs = mb.modbusASCII_readReg(0, MODBUS_INPUT_ELAPSED_TIME, 6);

                //e.Result = res;
            }
            catch (Exception ex)
            {
                e.Result = false;
            }

            mb.closePort();
            comm_semaphore.Release();
        }

        private void bgSetParams_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            SystemSounds.Beep.Play();
        }

        private void btnSet_Click(object sender, EventArgs e)
        {
            if (!bgSetParams.IsBusy)
            {
                paramsClass paramsSet = new paramsClass();

                paramsSet.autodrain_enabled = cbAutoDrain.Checked;
                paramsSet.autodrain_time = (ushort)numAutoDrainTime.Value;
                paramsSet.degas_pulse_off = (ushort)numDegasOff.Value;
                paramsSet.degas_pulse_on = (ushort)numDegasON.Value;
                paramsSet.degas_time = (ushort)numDegasDuration.Value;
                paramsSet.set_temp = (ushort)numSetTemp.Value;
                paramsSet.set_time = (ushort)(numCycleTimeH.Value * 60 + numCycleTimeM.Value);
                paramsSet.wlp_enabled = cbWLP.Checked;


                paramsLast.autodrain_enabled = paramsSet.autodrain_enabled;
                paramsLast.autodrain_time = paramsSet.autodrain_time;
                paramsLast.degas_pulse_off = paramsSet.degas_pulse_off;
                paramsLast.degas_pulse_on = paramsSet.degas_pulse_on;
                paramsLast.degas_time = paramsSet.degas_time;
                paramsLast.set_temp = paramsSet.set_temp;
                paramsLast.set_time = paramsSet.set_time;
                paramsLast.wlp_enabled = paramsSet.wlp_enabled;



                bgSetParams.RunWorkerAsync(paramsSet);
            }
        }
    }
}
