using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;

namespace si_auto_fwupdater
{
    public partial class Form1 : Form
    {

        modbus_ascii mb = new modbus_ascii();



        byte[] fwData = null;
        byte[] fwData_read = new byte[65536];


        bool verifyPend = false;


        public Form1()
        {
            InitializeComponent();
        }

        private void bgReadFW_DoWork(object sender, DoWorkEventArgs e)
        {

            try
            {

                mb.openPort();

                bgReadFW.WorkerReportsProgress = true;

                for (int i = 0; i < 512; i++)
                {
                    byte[] flashData = mb.modbusASCII_bootReadFlash(0, (uint)i * 128, 128);
                    Buffer.BlockCopy(flashData, 0, fwData_read, i * 128, 128);
                    bgReadFW.ReportProgress(((i + 1) * 100) / 512);
                }


                 e.Result = true;
            }
            catch (Exception ex)
            {
               e.Result = false;
            }


            mb.closePort();

           
        }

        private void bgReadFW_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            pb1.Value = e.ProgressPercentage;
        }

        private void bgReadFW_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            pb1.Value = 0;

            if ((bool)e.Result == false)
            {
                textBox1.AppendText("Error!" + System.Environment.NewLine);
                return;
            }

            textBox1.AppendText("OK!" + System.Environment.NewLine);

            if (verifyPend)
            {
                // Compare contents, ignoring first 2048 bytes

                bool verifyErr = false;

                for (int i=2048; i<fwData.Length; i++)
                {
                    if (fwData[i] != fwData_read[i])
                        verifyErr = true;
                }


                if (verifyErr)
                {
                    textBox1.AppendText("Verify error!" + System.Environment.NewLine);
                }
                else
                {
                    textBox1.AppendText("Verify OK!" + System.Environment.NewLine);
                }

            }
            else
            {
                SaveFileDialog sfd = new SaveFileDialog();

                sfd.Filter = "BIN file|*.bin";

                DialogResult res = sfd.ShowDialog();

                if (res == DialogResult.OK)
                {

                    try
                    {

                        System.IO.FileStream fw_filestream = new System.IO.FileStream(sfd.FileName, System.IO.FileMode.Create);

                        fw_filestream.Write(fwData_read, 0, fwData_read.Length);
                        fw_filestream.Flush();
                        fw_filestream.Close();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Error writing file!\r\n" + ex.Message);
                    }
                }
            }
        }

        private void btnCheck_Click(object sender, EventArgs e)
        {
            if ((bgErase.IsBusy) || (bgWrite.IsBusy) || (bgReadFW.IsBusy) || (bgCommCheck.IsBusy) || (bgReset.IsBusy))
                return;
            
            textBox1.AppendText("Checking comm...");
            bgCommCheck.RunWorkerAsync();
            
        }

        private void bgCommCheck_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                mb.openPort();
                byte[] id = mb.modbusASCII_bootCheckComm(0);
                
                e.Result = id;
            }
            catch (Exception ex)
            {
                e.Result = null;
            }

            mb.closePort();

        }

        private void bgCommCheck_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Result == null)
            {
                textBox1.AppendText(" fail!" + System.Environment.NewLine);
            }
            else
            {
                textBox1.AppendText(" OK!" + System.Environment.NewLine);


                UInt16 devID, revID;
                byte[] dat = (byte[])e.Result;

                devID = (ushort)(dat[0] << 8);
                devID |= dat[1];

                revID = (ushort)(dat[2] << 8);
                revID |= dat[3];


                textBox1.AppendText("Device ID: 0x" + devID.ToString("X2") + ", revision ID: " + revID.ToString("X2") + System.Environment.NewLine);

            }
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            if ((bgErase.IsBusy) || (bgWrite.IsBusy) || (bgReadFW.IsBusy) || (bgCommCheck.IsBusy) || (bgReset.IsBusy))
                return;

            textBox1.AppendText("Reset...");
            bgReset.RunWorkerAsync();
        }

        private void bgReset_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {

                mb.openPort();
                bool res = mb.modbusASCII_bootReset(0);


                e.Result = res;
            }
            catch (Exception ex)
            {
                e.Result = null;
            }

            mb.closePort();

        }

        private void bgReset_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if ((bool)e.Result == false)
            {
                textBox1.AppendText(" fail!" + System.Environment.NewLine);
            }
            else
            {
                textBox1.AppendText(" OK!" + System.Environment.NewLine);
            }
        }

        private void btnWrite_Click(object sender, EventArgs e)
        {

            if ((bgErase.IsBusy) || (bgWrite.IsBusy) || (bgReadFW.IsBusy) ||(bgCommCheck.IsBusy) || (bgReset.IsBusy))
                return;

            OpenFileDialog ofd = new OpenFileDialog();

            ofd.Filter = "BIN/HEX file|*.bin;*.hex";

            DialogResult res = ofd.ShowDialog();

            if (res == DialogResult.OK)
            {

                try
                {

                    System.IO.FileStream fw_filestream = new System.IO.FileStream(ofd.FileName, System.IO.FileMode.Open);


                    

                    if (System.IO.Path.GetExtension(ofd.FileName) == ".hex")
                    {

                        using (System.IO.StreamReader sr = new System.IO.StreamReader(fw_filestream))
                        {
                            string line;

                            fwData = new byte[65536];

                            // fill buffer with FF
                            for (int i = 0; i < fwData.Length; i++)
                                fwData[i] = 0xFF;

                            while ((line = sr.ReadLine()) != null)
                            {

                                byte checksumCalc = 0;

                                for (int i=1; i<line.Length; i+=2)
                                {
                                    checksumCalc += Convert.ToByte(line.Substring(i, 2), 16);
                                }

                                if (checksumCalc != 0)
                                {
                                    // checksum error
                                    MessageBox.Show("Checksum error!");
                                    return;
                                }

                                var dataLen = Convert.ToByte(line.Substring(1, 2),16);
                                var addr = Convert.ToUInt16(line.Substring(3, 4), 16);
                                var recordType = Convert.ToByte(line.Substring(7, 2), 16);



                                if (recordType != 0)
                                {
                                    // Unknown record type, stop reading
                                    break;
                                }



                                if (dataLen > 0)
                                {
                                    var dataStr = line.Substring(9, dataLen * 2);

                                    byte[] dataBytes = new byte[dataLen];
                                    for (int i = 0; i < dataStr.Length; i += 2)
                                        dataBytes[i / 2] = Convert.ToByte(dataStr.Substring(i, 2), 16);

                                    Buffer.BlockCopy(dataBytes, 0, fwData, addr, dataLen);

                                }



                            }
                        }
                    }
                    else
                    {
                        if (fw_filestream.Length > 65536)
                        {
                            MessageBox.Show("File too large!");
                            return;
                        }
                        fwData = new byte[fw_filestream.Length];
                        fw_filestream.Read(fwData, 0, fwData.Length);

                    }











                    fw_filestream.Close();


                    textBox1.AppendText("Erasing flash...");
                    bgErase.RunWorkerAsync();
                    


                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error reading file!");
                }


            }


        }

        private void bgErase_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {

                mb.openPort();

                bgErase.WorkerReportsProgress = true;

                for (int i = 8; i < 256; i++)
                {
                    bool res = mb.modbusASCII_bootEraseFlash(0, (byte)i);

                    if (!res)
                    {
                        e.Result = false;
                        mb.closePort();
                        return;
                    }

                    bgErase.ReportProgress(((i + 1) * 100) / 256);
                }

                e.Result = true;
                

            }
            catch (Exception ex)
            {
                e.Result = false;
            }

            mb.closePort();

        }

        private void bgErase_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            pb1.Value = e.ProgressPercentage;
        }

        private void bgErase_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            pb1.Value = 0;

            if ((bool)e.Result == false)
            {
                textBox1.AppendText(" fail!" + System.Environment.NewLine);
            }
            else
            {
                textBox1.AppendText(" OK!" + System.Environment.NewLine);
                textBox1.AppendText("Writing flash...");
                bgWrite.RunWorkerAsync();
            }
        }

        private void bgWrite_DoWork(object sender, DoWorkEventArgs e)
        {

            try
            {

            

                byte[] fwBlock = new byte[128];

                mb.openPort();

                bgWrite.WorkerReportsProgress = true;

                for (int i = 16; i < 512; i++)
                {

                    Buffer.BlockCopy(fwData, i * 128, fwBlock, 0, 128);

                    bool blockHasData = false;

                    for (int j = 0; j < 128; j++)
                    {
                        if (fwBlock[j] != 0xFF)
                        {
                            blockHasData = true;
                            break;
                        }
                    }

                    if (blockHasData)
                    {
                        bool res = mb.modbusASCII_bootWriteFlash(0, (uint)i * 128, 128, fwBlock);

                        if (!res)
                        {
                            e.Result = false;
                            mb.closePort();
                            return;
                        }
                    }

                    bgWrite.ReportProgress(((i + 1) * 100) / 512);
                }

                e.Result = true;
            }
            catch (Exception ex)
            {
                e.Result = false;
            }

            mb.closePort();

        }

        private void bgWrite_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            pb1.Value = e.ProgressPercentage;
        }

        private void bgWrite_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            pb1.Value = 0;

            if ((bool)e.Result == false)
            {
                textBox1.AppendText(" fail!" + System.Environment.NewLine);
            }
            else
            {
                textBox1.AppendText(" OK!" + System.Environment.NewLine);
                textBox1.AppendText("Reading flash...");
                verifyPend = true;
                bgReadFW.RunWorkerAsync();
            }
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

        private void cbComPort_SelectedIndexChanged(object sender, EventArgs e)
        {
            cbComPort.SelectedIndex = cbComPort.FindStringExact(mb.setPort(cbComPort.Text));
        }

        private void btnRead_Click(object sender, EventArgs e)
        {
            if ((bgErase.IsBusy) || (bgWrite.IsBusy) || (bgReadFW.IsBusy) || (bgCommCheck.IsBusy) || (bgReset.IsBusy))
                return;

            textBox1.AppendText("Reading flash...");
            verifyPend = false;
            bgReadFW.RunWorkerAsync();
        }
    }
}
