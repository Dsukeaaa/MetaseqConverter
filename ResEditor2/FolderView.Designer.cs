namespace ResEditor2
{
	partial class FolderView
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

		#region コンポーネント デザイナーで生成されたコード

		/// <summary> 
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を 
		/// コード エディターで変更しないでください。
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FolderView));
			this.listView1 = new System.Windows.Forms.ListView();
			this.imageList2 = new System.Windows.Forms.ImageList(this.components);
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.treeView1 = new System.Windows.Forms.TreeView();
			this.backButton = new System.Windows.Forms.Button();
			this.forwardButton = new System.Windows.Forms.Button();
			this.addressTextBox = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// listView1
			// 
			this.listView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.listView1.LargeImageList = this.imageList2;
			this.listView1.Location = new System.Drawing.Point(246, 32);
			this.listView1.Name = "listView1";
			this.listView1.Size = new System.Drawing.Size(444, 368);
			this.listView1.TabIndex = 0;
			this.listView1.UseCompatibleStateImageBehavior = false;
			this.listView1.ItemActivate += new System.EventHandler(this.listView1_ItemActivate);
			this.listView1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listView1_KeyDown);
			// 
			// imageList2
			// 
			this.imageList2.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList2.ImageStream")));
			this.imageList2.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList2.Images.SetKeyName(0, "folderIcon0.png");
			this.imageList2.Images.SetKeyName(1, "fileIcon0.png");
			// 
			// imageList1
			// 
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList1.Images.SetKeyName(0, "folderIcon0.png");
			this.imageList1.Images.SetKeyName(1, "fileIcon0.png");
			// 
			// treeView1
			// 
			this.treeView1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
			this.treeView1.ImageIndex = 0;
			this.treeView1.ImageList = this.imageList1;
			this.treeView1.Location = new System.Drawing.Point(0, 32);
			this.treeView1.Name = "treeView1";
			this.treeView1.SelectedImageIndex = 0;
			this.treeView1.Size = new System.Drawing.Size(240, 368);
			this.treeView1.TabIndex = 1;
			this.treeView1.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeView1_BeforeExpand);
			this.treeView1.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeView1_BeforeSelect);
			this.treeView1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.treeView1_KeyDown);
			// 
			// backButton
			// 
			this.backButton.BackColor = System.Drawing.SystemColors.Control;
			this.backButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.backButton.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.backButton.Location = new System.Drawing.Point(3, 3);
			this.backButton.Name = "backButton";
			this.backButton.Size = new System.Drawing.Size(25, 25);
			this.backButton.TabIndex = 2;
			this.backButton.Text = "←";
			this.backButton.UseVisualStyleBackColor = false;
			this.backButton.Click += new System.EventHandler(this.backButton_Click);
			this.backButton.Paint += new System.Windows.Forms.PaintEventHandler(this.backButton_Paint);
			// 
			// forwardButton
			// 
			this.forwardButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.forwardButton.ForeColor = System.Drawing.SystemColors.GradientActiveCaption;
			this.forwardButton.Location = new System.Drawing.Point(34, 3);
			this.forwardButton.Name = "forwardButton";
			this.forwardButton.Size = new System.Drawing.Size(25, 25);
			this.forwardButton.TabIndex = 3;
			this.forwardButton.Text = "→";
			this.forwardButton.UseVisualStyleBackColor = true;
			this.forwardButton.Click += new System.EventHandler(this.forwardButton_Click);
			this.forwardButton.Paint += new System.Windows.Forms.PaintEventHandler(this.forwardButton_Paint);
			// 
			// addressTextBox
			// 
			this.addressTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.addressTextBox.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.addressTextBox.Location = new System.Drawing.Point(64, 4);
			this.addressTextBox.Name = "addressTextBox";
			this.addressTextBox.Size = new System.Drawing.Size(625, 23);
			this.addressTextBox.TabIndex = 4;
			this.addressTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.addressTextBox_KeyDown);
			// 
			// FolderView
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSize = true;
			this.Controls.Add(this.addressTextBox);
			this.Controls.Add(this.forwardButton);
			this.Controls.Add(this.backButton);
			this.Controls.Add(this.treeView1);
			this.Controls.Add(this.listView1);
			this.Name = "FolderView";
			this.Size = new System.Drawing.Size(703, 408);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListView listView1;
		private System.Windows.Forms.TreeView treeView1;
		private System.Windows.Forms.ImageList imageList1;
		private System.Windows.Forms.Button backButton;
		private System.Windows.Forms.Button forwardButton;
		private System.Windows.Forms.ImageList imageList2;
		private System.Windows.Forms.TextBox addressTextBox;
	}
}
