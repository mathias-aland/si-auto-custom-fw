
namespace si_auto_fwupdater
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
            this.btnRead = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.bgReadFW = new System.ComponentModel.BackgroundWorker();
            this.pb1 = new System.Windows.Forms.ProgressBar();
            this.btnReset = new System.Windows.Forms.Button();
            this.btnWrite = new System.Windows.Forms.Button();
            this.btnCheck = new System.Windows.Forms.Button();
            this.bgCommCheck = new System.ComponentModel.BackgroundWorker();
            this.bgReset = new System.ComponentModel.BackgroundWorker();
            this.bgErase = new System.ComponentModel.BackgroundWorker();
            this.bgWrite = new System.ComponentModel.BackgroundWorker();
            this.cbComPort = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // btnRead
            // 
            this.btnRead.Location = new System.Drawing.Point(184, 12);
            this.btnRead.Name = "btnRead";
            this.btnRead.Size = new System.Drawing.Size(80, 23);
            this.btnRead.TabIndex = 3;
            this.btnRead.Text = "Read flash";
            this.btnRead.UseVisualStyleBackColor = true;
            this.btnRead.Click += new System.EventHandler(this.btnRead_Click);
            // 
            // textBox1
            // 
            this.textBox1.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBox1.Location = new System.Drawing.Point(12, 70);
            this.textBox1.MaxLength = 0;
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(338, 173);
            this.textBox1.TabIndex = 7;
            this.textBox1.WordWrap = false;
            // 
            // bgReadFW
            // 
            this.bgReadFW.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgReadFW_DoWork);
            this.bgReadFW.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgReadFW_ProgressChanged);
            this.bgReadFW.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgReadFW_RunWorkerCompleted);
            // 
            // pb1
            // 
            this.pb1.Location = new System.Drawing.Point(98, 41);
            this.pb1.Name = "pb1";
            this.pb1.Size = new System.Drawing.Size(252, 21);
            this.pb1.TabIndex = 6;
            // 
            // btnReset
            // 
            this.btnReset.Location = new System.Drawing.Point(98, 12);
            this.btnReset.Name = "btnReset";
            this.btnReset.Size = new System.Drawing.Size(80, 23);
            this.btnReset.TabIndex = 2;
            this.btnReset.Text = "Reset";
            this.btnReset.UseVisualStyleBackColor = true;
            this.btnReset.Click += new System.EventHandler(this.btnReset_Click);
            // 
            // btnWrite
            // 
            this.btnWrite.Location = new System.Drawing.Point(270, 12);
            this.btnWrite.Name = "btnWrite";
            this.btnWrite.Size = new System.Drawing.Size(80, 23);
            this.btnWrite.TabIndex = 4;
            this.btnWrite.Text = "Write flash";
            this.btnWrite.UseVisualStyleBackColor = true;
            this.btnWrite.Click += new System.EventHandler(this.btnWrite_Click);
            // 
            // btnCheck
            // 
            this.btnCheck.Location = new System.Drawing.Point(12, 12);
            this.btnCheck.Name = "btnCheck";
            this.btnCheck.Size = new System.Drawing.Size(80, 23);
            this.btnCheck.TabIndex = 1;
            this.btnCheck.Text = "Check comm";
            this.btnCheck.UseVisualStyleBackColor = true;
            this.btnCheck.Click += new System.EventHandler(this.btnCheck_Click);
            // 
            // bgCommCheck
            // 
            this.bgCommCheck.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgCommCheck_DoWork);
            this.bgCommCheck.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgCommCheck_RunWorkerCompleted);
            // 
            // bgReset
            // 
            this.bgReset.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgReset_DoWork);
            this.bgReset.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgReset_RunWorkerCompleted);
            // 
            // bgErase
            // 
            this.bgErase.WorkerReportsProgress = true;
            this.bgErase.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgErase_DoWork);
            this.bgErase.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgErase_ProgressChanged);
            this.bgErase.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgErase_RunWorkerCompleted);
            // 
            // bgWrite
            // 
            this.bgWrite.WorkerReportsProgress = true;
            this.bgWrite.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgWrite_DoWork);
            this.bgWrite.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgWrite_ProgressChanged);
            this.bgWrite.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgWrite_RunWorkerCompleted);
            // 
            // cbComPort
            // 
            this.cbComPort.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbComPort.FormattingEnabled = true;
            this.cbComPort.Location = new System.Drawing.Point(12, 41);
            this.cbComPort.Name = "cbComPort";
            this.cbComPort.Size = new System.Drawing.Size(80, 21);
            this.cbComPort.TabIndex = 5;
            this.cbComPort.DropDown += new System.EventHandler(this.cbComPort_DropDown);
            this.cbComPort.SelectedIndexChanged += new System.EventHandler(this.cbComPort_SelectedIndexChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(364, 255);
            this.Controls.Add(this.cbComPort);
            this.Controls.Add(this.btnCheck);
            this.Controls.Add(this.btnWrite);
            this.Controls.Add(this.btnReset);
            this.Controls.Add(this.pb1);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.btnRead);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "SI Auto custom FW flasher";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnRead;
        private System.Windows.Forms.TextBox textBox1;
        private System.ComponentModel.BackgroundWorker bgReadFW;
        private System.Windows.Forms.ProgressBar pb1;
        private System.Windows.Forms.Button btnReset;
        private System.Windows.Forms.Button btnWrite;
        private System.Windows.Forms.Button btnCheck;
        private System.ComponentModel.BackgroundWorker bgCommCheck;
        private System.ComponentModel.BackgroundWorker bgReset;
        private System.ComponentModel.BackgroundWorker bgErase;
        private System.ComponentModel.BackgroundWorker bgWrite;
        private System.Windows.Forms.ComboBox cbComPort;
    }
}

