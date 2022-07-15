namespace ResEditor
{
    partial class Form1
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
			this.resView = new System.Windows.Forms.TreeView();
			this.ConvertButton = new System.Windows.Forms.Button();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.ConvertProgress = new System.Windows.Forms.Label();
			this.previewLabel = new System.Windows.Forms.Label();
			this.resViewContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.ExplolerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.resViewContextMenu.SuspendLayout();
			this.SuspendLayout();
			// 
			// resView
			// 
			this.resView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.resView.ContextMenuStrip = this.resViewContextMenu;
			this.resView.ImageIndex = 0;
			this.resView.ImageList = this.imageList1;
			this.resView.Location = new System.Drawing.Point(12, 48);
			this.resView.Name = "resView";
			this.resView.SelectedImageIndex = 0;
			this.resView.Size = new System.Drawing.Size(337, 439);
			this.resView.TabIndex = 0;
			this.resView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
			this.resView.MouseDown += new System.Windows.Forms.MouseEventHandler(this.resView_MouseDown);
			// 
			// ConvertButton
			// 
			this.ConvertButton.Location = new System.Drawing.Point(12, 12);
			this.ConvertButton.Name = "ConvertButton";
			this.ConvertButton.Size = new System.Drawing.Size(75, 23);
			this.ConvertButton.TabIndex = 1;
			this.ConvertButton.Text = "変換開始";
			this.ConvertButton.UseVisualStyleBackColor = true;
			this.ConvertButton.Click += new System.EventHandler(this.ConvertButton_Click);
			// 
			// imageList1
			// 
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList1.Images.SetKeyName(0, "folderIcon0.png");
			this.imageList1.Images.SetKeyName(1, "fileIcon0.png");
			// 
			// ConvertProgress
			// 
			this.ConvertProgress.AutoSize = true;
			this.ConvertProgress.Font = new System.Drawing.Font("MS UI Gothic", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.ConvertProgress.Location = new System.Drawing.Point(93, 16);
			this.ConvertProgress.Name = "ConvertProgress";
			this.ConvertProgress.Size = new System.Drawing.Size(67, 15);
			this.ConvertProgress.TabIndex = 2;
			this.ConvertProgress.Text = "変換状況";
			// 
			// previewLabel
			// 
			this.previewLabel.AutoSize = true;
			this.previewLabel.Location = new System.Drawing.Point(355, 48);
			this.previewLabel.Name = "previewLabel";
			this.previewLabel.Size = new System.Drawing.Size(0, 12);
			this.previewLabel.TabIndex = 4;
			// 
			// resViewContextMenu
			// 
			this.resViewContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ExplolerToolStripMenuItem});
			this.resViewContextMenu.Name = "resViewContextMenu";
			this.resViewContextMenu.Size = new System.Drawing.Size(209, 26);
			// 
			// ExplolerToolStripMenuItem
			// 
			this.ExplolerToolStripMenuItem.Name = "ExplolerToolStripMenuItem";
			this.ExplolerToolStripMenuItem.Size = new System.Drawing.Size(208, 22);
			this.ExplolerToolStripMenuItem.Text = "エクスプローラーで表示";
			this.ExplolerToolStripMenuItem.Click += new System.EventHandler(this.ExplolerToolStripMenuItem_Click);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(679, 499);
			this.Controls.Add(this.previewLabel);
			this.Controls.Add(this.ConvertProgress);
			this.Controls.Add(this.ConvertButton);
			this.Controls.Add(this.resView);
			this.Name = "Form1";
			this.Text = "Form1";
			this.resViewContextMenu.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TreeView resView;
        private System.Windows.Forms.Button ConvertButton;
		private System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.Label ConvertProgress;
		private System.Windows.Forms.Label previewLabel;
		private System.Windows.Forms.ContextMenuStrip resViewContextMenu;
		private System.Windows.Forms.ToolStripMenuItem ExplolerToolStripMenuItem;
    }
}

