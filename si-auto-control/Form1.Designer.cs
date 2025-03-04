
namespace si_auto_control
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.cbComPort = new System.Windows.Forms.ComboBox();
            this.numCycleTimeH = new System.Windows.Forms.NumericUpDown();
            this.numCycleTimeM = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.numSetTemp = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.lblIn2State = new System.Windows.Forms.Label();
            this.lblIn1State = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.lblOut2State = new System.Windows.Forms.Label();
            this.lblOut1State = new System.Windows.Forms.Label();
            this.btnStart = new System.Windows.Forms.Button();
            this.btnDegas = new System.Windows.Forms.Button();
            this.btnDrain = new System.Windows.Forms.Button();
            this.btnStop = new System.Windows.Forms.Button();
            this.btnHeat = new System.Windows.Forms.Button();
            this.btnRecirc = new System.Windows.Forms.Button();
            this.btnAir = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.numDegasOff = new System.Windows.Forms.NumericUpDown();
            this.numDegasON = new System.Windows.Forms.NumericUpDown();
            this.numDegasDuration = new System.Windows.Forms.NumericUpDown();
            this.label12 = new System.Windows.Forms.Label();
            this.numAutoDrainTime = new System.Windows.Forms.NumericUpDown();
            this.label13 = new System.Windows.Forms.Label();
            this.cbAutoDrain = new System.Windows.Forms.CheckBox();
            this.label14 = new System.Windows.Forms.Label();
            this.cbWLP = new System.Windows.Forms.CheckBox();
            this.btnConnect = new System.Windows.Forms.Button();
            this.label15 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.lblState = new System.Windows.Forms.Label();
            this.lblState2 = new System.Windows.Forms.Label();
            this.lblWLVL = new System.Windows.Forms.Label();
            this.lblT2 = new System.Windows.Forms.Label();
            this.lblT1 = new System.Windows.Forms.Label();
            this.tmrRead = new System.Windows.Forms.Timer(this.components);
            this.bgReadData = new System.ComponentModel.BackgroundWorker();
            this.label1 = new System.Windows.Forms.Label();
            this.bgSetParams = new System.ComponentModel.BackgroundWorker();
            this.btnSet = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.numCycleTimeH)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numCycleTimeM)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numSetTemp)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numDegasOff)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numDegasON)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numDegasDuration)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAutoDrainTime)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // cbComPort
            // 
            this.cbComPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbComPort.FormattingEnabled = true;
            this.cbComPort.Location = new System.Drawing.Point(12, 28);
            this.cbComPort.Name = "cbComPort";
            this.cbComPort.Size = new System.Drawing.Size(80, 21);
            this.cbComPort.TabIndex = 6;
            this.cbComPort.DropDown += new System.EventHandler(this.cbComPort_DropDown);
            this.cbComPort.SelectedIndexChanged += new System.EventHandler(this.cbComPort_SelectedIndexChanged);
            // 
            // numCycleTimeH
            // 
            this.numCycleTimeH.Location = new System.Drawing.Point(203, 28);
            this.numCycleTimeH.Maximum = new decimal(new int[] {
            9,
            0,
            0,
            0});
            this.numCycleTimeH.Name = "numCycleTimeH";
            this.numCycleTimeH.Size = new System.Drawing.Size(48, 20);
            this.numCycleTimeH.TabIndex = 7;
            // 
            // numCycleTimeM
            // 
            this.numCycleTimeM.Location = new System.Drawing.Point(257, 28);
            this.numCycleTimeM.Maximum = new decimal(new int[] {
            59,
            0,
            0,
            0});
            this.numCycleTimeM.Name = "numCycleTimeM";
            this.numCycleTimeM.Size = new System.Drawing.Size(55, 20);
            this.numCycleTimeM.TabIndex = 7;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(200, 11);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(112, 13);
            this.label2.TabIndex = 8;
            this.label2.Text = "Cycle duration (H:MM)";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 12);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(52, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "COM port";
            // 
            // numSetTemp
            // 
            this.numSetTemp.Location = new System.Drawing.Point(327, 28);
            this.numSetTemp.Maximum = new decimal(new int[] {
            95,
            0,
            0,
            0});
            this.numSetTemp.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numSetTemp.Name = "numSetTemp";
            this.numSetTemp.Size = new System.Drawing.Size(66, 20);
            this.numSetTemp.TabIndex = 7;
            this.numSetTemp.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(324, 11);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(69, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Set temp (°C)";
            // 
            // lblIn2State
            // 
            this.lblIn2State.AutoSize = true;
            this.lblIn2State.Font = new System.Drawing.Font("Consolas", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblIn2State.Location = new System.Drawing.Point(8, 18);
            this.lblIn2State.Name = "lblIn2State";
            this.lblIn2State.Size = new System.Drawing.Size(116, 28);
            this.lblIn2State.TabIndex = 10;
            this.lblIn2State.Text = "--------";
            // 
            // lblIn1State
            // 
            this.lblIn1State.AutoSize = true;
            this.lblIn1State.Font = new System.Drawing.Font("Consolas", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblIn1State.Location = new System.Drawing.Point(8, 46);
            this.lblIn1State.Name = "lblIn1State";
            this.lblIn1State.Size = new System.Drawing.Size(116, 28);
            this.lblIn1State.TabIndex = 10;
            this.lblIn1State.Text = "--------";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.lblIn2State);
            this.groupBox1.Controls.Add(this.lblIn1State);
            this.groupBox1.Location = new System.Drawing.Point(274, 154);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(132, 87);
            this.groupBox1.TabIndex = 11;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Inputs";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.lblOut2State);
            this.groupBox2.Controls.Add(this.lblOut1State);
            this.groupBox2.Location = new System.Drawing.Point(277, 247);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(132, 87);
            this.groupBox2.TabIndex = 11;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Outputs";
            // 
            // lblOut2State
            // 
            this.lblOut2State.AutoSize = true;
            this.lblOut2State.Font = new System.Drawing.Font("Consolas", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblOut2State.Location = new System.Drawing.Point(8, 18);
            this.lblOut2State.Name = "lblOut2State";
            this.lblOut2State.Size = new System.Drawing.Size(116, 28);
            this.lblOut2State.TabIndex = 10;
            this.lblOut2State.Text = "--------";
            // 
            // lblOut1State
            // 
            this.lblOut1State.AutoSize = true;
            this.lblOut1State.Font = new System.Drawing.Font("Consolas", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblOut1State.Location = new System.Drawing.Point(8, 46);
            this.lblOut1State.Name = "lblOut1State";
            this.lblOut1State.Size = new System.Drawing.Size(116, 28);
            this.lblOut1State.TabIndex = 10;
            this.lblOut1State.Text = "--------";
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(415, 166);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(75, 23);
            this.btnStart.TabIndex = 12;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            // 
            // btnDegas
            // 
            this.btnDegas.Location = new System.Drawing.Point(415, 195);
            this.btnDegas.Name = "btnDegas";
            this.btnDegas.Size = new System.Drawing.Size(75, 23);
            this.btnDegas.TabIndex = 12;
            this.btnDegas.Text = "Degas";
            this.btnDegas.UseVisualStyleBackColor = true;
            // 
            // btnDrain
            // 
            this.btnDrain.Location = new System.Drawing.Point(415, 224);
            this.btnDrain.Name = "btnDrain";
            this.btnDrain.Size = new System.Drawing.Size(75, 23);
            this.btnDrain.TabIndex = 12;
            this.btnDrain.Text = "Drain";
            this.btnDrain.UseVisualStyleBackColor = true;
            // 
            // btnStop
            // 
            this.btnStop.Location = new System.Drawing.Point(415, 137);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(75, 23);
            this.btnStop.TabIndex = 12;
            this.btnStop.Text = "Stop";
            this.btnStop.UseVisualStyleBackColor = true;
            // 
            // btnHeat
            // 
            this.btnHeat.Location = new System.Drawing.Point(415, 253);
            this.btnHeat.Name = "btnHeat";
            this.btnHeat.Size = new System.Drawing.Size(75, 23);
            this.btnHeat.TabIndex = 12;
            this.btnHeat.Text = "Heat";
            this.btnHeat.UseVisualStyleBackColor = true;
            // 
            // btnRecirc
            // 
            this.btnRecirc.Location = new System.Drawing.Point(415, 282);
            this.btnRecirc.Name = "btnRecirc";
            this.btnRecirc.Size = new System.Drawing.Size(75, 23);
            this.btnRecirc.TabIndex = 12;
            this.btnRecirc.Text = "Recirc";
            this.btnRecirc.UseVisualStyleBackColor = true;
            // 
            // btnAir
            // 
            this.btnAir.Location = new System.Drawing.Point(415, 311);
            this.btnAir.Name = "btnAir";
            this.btnAir.Size = new System.Drawing.Size(75, 23);
            this.btnAir.TabIndex = 12;
            this.btnAir.Text = "Air";
            this.btnAir.UseVisualStyleBackColor = true;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(13, 74);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(79, 13);
            this.label6.TabIndex = 8;
            this.label6.Text = "Degas duration";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(95, 74);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(75, 13);
            this.label8.TabIndex = 8;
            this.label8.Text = "Degas on time";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(177, 74);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(75, 13);
            this.label11.TabIndex = 8;
            this.label11.Text = "Degas off time";
            // 
            // numDegasOff
            // 
            this.numDegasOff.Location = new System.Drawing.Point(180, 90);
            this.numDegasOff.Maximum = new decimal(new int[] {
            99,
            0,
            0,
            0});
            this.numDegasOff.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numDegasOff.Name = "numDegasOff";
            this.numDegasOff.Size = new System.Drawing.Size(47, 20);
            this.numDegasOff.TabIndex = 7;
            this.numDegasOff.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // numDegasON
            // 
            this.numDegasON.Location = new System.Drawing.Point(98, 90);
            this.numDegasON.Maximum = new decimal(new int[] {
            99,
            0,
            0,
            0});
            this.numDegasON.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numDegasON.Name = "numDegasON";
            this.numDegasON.Size = new System.Drawing.Size(47, 20);
            this.numDegasON.TabIndex = 7;
            this.numDegasON.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // numDegasDuration
            // 
            this.numDegasDuration.Location = new System.Drawing.Point(16, 90);
            this.numDegasDuration.Maximum = new decimal(new int[] {
            95,
            0,
            0,
            0});
            this.numDegasDuration.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numDegasDuration.Name = "numDegasDuration";
            this.numDegasDuration.Size = new System.Drawing.Size(47, 20);
            this.numDegasDuration.TabIndex = 7;
            this.numDegasDuration.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(259, 74);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(77, 13);
            this.label12.TabIndex = 8;
            this.label12.Text = "Auto drain time";
            // 
            // numAutoDrainTime
            // 
            this.numAutoDrainTime.Location = new System.Drawing.Point(262, 90);
            this.numAutoDrainTime.Maximum = new decimal(new int[] {
            95,
            0,
            0,
            0});
            this.numAutoDrainTime.Minimum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numAutoDrainTime.Name = "numAutoDrainTime";
            this.numAutoDrainTime.Size = new System.Drawing.Size(47, 20);
            this.numAutoDrainTime.TabIndex = 7;
            this.numAutoDrainTime.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(341, 75);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(55, 13);
            this.label13.TabIndex = 8;
            this.label13.Text = "Auto drain";
            // 
            // cbAutoDrain
            // 
            this.cbAutoDrain.AutoSize = true;
            this.cbAutoDrain.Location = new System.Drawing.Point(344, 93);
            this.cbAutoDrain.Name = "cbAutoDrain";
            this.cbAutoDrain.Size = new System.Drawing.Size(65, 17);
            this.cbAutoDrain.TabIndex = 13;
            this.cbAutoDrain.Text = "Enabled";
            this.cbAutoDrain.UseVisualStyleBackColor = true;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(412, 75);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(85, 13);
            this.label14.TabIndex = 8;
            this.label14.Text = "Water level prot.";
            // 
            // cbWLP
            // 
            this.cbWLP.AutoSize = true;
            this.cbWLP.Location = new System.Drawing.Point(415, 93);
            this.cbWLP.Name = "cbWLP";
            this.cbWLP.Size = new System.Drawing.Size(65, 17);
            this.cbWLP.TabIndex = 13;
            this.cbWLP.Text = "Enabled";
            this.cbWLP.UseVisualStyleBackColor = true;
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(98, 27);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(75, 23);
            this.btnConnect.TabIndex = 12;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(66, 93);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(23, 13);
            this.label15.TabIndex = 8;
            this.label15.Text = "min";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(148, 93);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(24, 13);
            this.label16.TabIndex = 8;
            this.label16.Text = "sec";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(312, 93);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(24, 13);
            this.label17.TabIndex = 8;
            this.label17.Text = "sec";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(229, 93);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(24, 13);
            this.label18.TabIndex = 8;
            this.label18.Text = "sec";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.lblState);
            this.groupBox3.Controls.Add(this.lblState2);
            this.groupBox3.Controls.Add(this.lblWLVL);
            this.groupBox3.Controls.Add(this.lblT2);
            this.groupBox3.Controls.Add(this.lblT1);
            this.groupBox3.Location = new System.Drawing.Point(12, 154);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(256, 180);
            this.groupBox3.TabIndex = 11;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "State";
            // 
            // lblState
            // 
            this.lblState.AutoSize = true;
            this.lblState.Font = new System.Drawing.Font("Consolas", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblState.Location = new System.Drawing.Point(6, 16);
            this.lblState.Name = "lblState";
            this.lblState.Size = new System.Drawing.Size(103, 28);
            this.lblState.TabIndex = 10;
            this.lblState.Text = "UNKNOWN";
            // 
            // lblState2
            // 
            this.lblState2.AutoSize = true;
            this.lblState2.Font = new System.Drawing.Font("Consolas", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblState2.Location = new System.Drawing.Point(6, 136);
            this.lblState2.Name = "lblState2";
            this.lblState2.Size = new System.Drawing.Size(0, 28);
            this.lblState2.TabIndex = 10;
            // 
            // lblWLVL
            // 
            this.lblWLVL.AutoSize = true;
            this.lblWLVL.Font = new System.Drawing.Font("Consolas", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblWLVL.Location = new System.Drawing.Point(6, 106);
            this.lblWLVL.Name = "lblWLVL";
            this.lblWLVL.Size = new System.Drawing.Size(207, 28);
            this.lblWLVL.TabIndex = 10;
            this.lblWLVL.Text = "W. LEVEL: - - -";
            // 
            // lblT2
            // 
            this.lblT2.AutoSize = true;
            this.lblT2.Font = new System.Drawing.Font("Consolas", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblT2.Location = new System.Drawing.Point(6, 76);
            this.lblT2.Name = "lblT2";
            this.lblT2.Size = new System.Drawing.Size(129, 28);
            this.lblT2.TabIndex = 10;
            this.lblT2.Text = "T2: -- °C";
            // 
            // lblT1
            // 
            this.lblT1.AutoSize = true;
            this.lblT1.Font = new System.Drawing.Font("Consolas", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblT1.Location = new System.Drawing.Point(6, 46);
            this.lblT1.Name = "lblT1";
            this.lblT1.Size = new System.Drawing.Size(129, 28);
            this.lblT1.TabIndex = 10;
            this.lblT1.Text = "T1: -- °C";
            // 
            // tmrRead
            // 
            this.tmrRead.Tick += new System.EventHandler(this.tmrRead_Tick);
            // 
            // bgReadData
            // 
            this.bgReadData.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgReadData_DoWork);
            this.bgReadData.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgReadData_RunWorkerCompleted);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(26, 350);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 14;
            this.label1.Text = "label1";
            // 
            // bgSetParams
            // 
            this.bgSetParams.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgSetParams_DoWork);
            this.bgSetParams.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgSetParams_RunWorkerCompleted);
            // 
            // btnSet
            // 
            this.btnSet.Location = new System.Drawing.Point(416, 12);
            this.btnSet.Name = "btnSet";
            this.btnSet.Size = new System.Drawing.Size(75, 48);
            this.btnSet.TabIndex = 12;
            this.btnSet.Text = "Set";
            this.btnSet.Click += new System.EventHandler(this.btnSet_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(503, 384);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.cbWLP);
            this.Controls.Add(this.cbAutoDrain);
            this.Controls.Add(this.btnDegas);
            this.Controls.Add(this.btnAir);
            this.Controls.Add(this.btnRecirc);
            this.Controls.Add(this.btnHeat);
            this.Controls.Add(this.btnDrain);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.btnSet);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.btnStart);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label14);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label18);
            this.Controls.Add(this.label17);
            this.Controls.Add(this.label16);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.numCycleTimeM);
            this.Controls.Add(this.numDegasDuration);
            this.Controls.Add(this.numDegasON);
            this.Controls.Add(this.numAutoDrainTime);
            this.Controls.Add(this.numDegasOff);
            this.Controls.Add(this.numSetTemp);
            this.Controls.Add(this.numCycleTimeH);
            this.Controls.Add(this.cbComPort);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.numCycleTimeH)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numCycleTimeM)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numSetTemp)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numDegasOff)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numDegasON)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numDegasDuration)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAutoDrainTime)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox cbComPort;
        private System.Windows.Forms.NumericUpDown numCycleTimeH;
        private System.Windows.Forms.NumericUpDown numCycleTimeM;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown numSetTemp;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label lblIn2State;
        private System.Windows.Forms.Label lblIn1State;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label lblOut2State;
        private System.Windows.Forms.Label lblOut1State;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnDegas;
        private System.Windows.Forms.Button btnDrain;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.Button btnHeat;
        private System.Windows.Forms.Button btnRecirc;
        private System.Windows.Forms.Button btnAir;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.NumericUpDown numDegasOff;
        private System.Windows.Forms.NumericUpDown numDegasON;
        private System.Windows.Forms.NumericUpDown numDegasDuration;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.NumericUpDown numAutoDrainTime;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.CheckBox cbAutoDrain;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.CheckBox cbWLP;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label lblState;
        private System.Windows.Forms.Label lblState2;
        private System.Windows.Forms.Label lblWLVL;
        private System.Windows.Forms.Label lblT2;
        private System.Windows.Forms.Label lblT1;
        private System.Windows.Forms.Timer tmrRead;
        private System.ComponentModel.BackgroundWorker bgReadData;
        private System.Windows.Forms.Label label1;
        private System.ComponentModel.BackgroundWorker bgSetParams;
        private System.Windows.Forms.Button btnSet;
    }
}

