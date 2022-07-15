namespace ResEditor2
{
    partial class MainForm
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
			this.folderView1 = new ResEditor2.FolderView();
			this.convertStart1 = new ResEditor2.ConvertStart();
			this.SuspendLayout();
			// 
			// folderView1
			// 
			this.folderView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.folderView1.AutoSize = true;
			this.folderView1.Location = new System.Drawing.Point(12, 33);
			this.folderView1.Name = "folderView1";
			this.folderView1.Size = new System.Drawing.Size(698, 424);
			this.folderView1.TabIndex = 1;
			// 
			// convertStart1
			// 
			this.convertStart1.Location = new System.Drawing.Point(12, 2);
			this.convertStart1.Name = "convertStart1";
			this.convertStart1.Size = new System.Drawing.Size(89, 32);
			this.convertStart1.TabIndex = 0;
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(715, 466);
			this.Controls.Add(this.folderView1);
			this.Controls.Add(this.convertStart1);
			this.Name = "MainForm";
			this.Text = "ResEditor2";
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private ConvertStart convertStart1;
		private FolderView folderView1;

    }
}

