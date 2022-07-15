namespace ResEditor2
{
    partial class ConvertStart
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
			this.StartConvButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// StartConvButton
			// 
			this.StartConvButton.Location = new System.Drawing.Point(3, 3);
			this.StartConvButton.Name = "StartConvButton";
			this.StartConvButton.Size = new System.Drawing.Size(75, 23);
			this.StartConvButton.TabIndex = 0;
			this.StartConvButton.Text = "変換";
			this.StartConvButton.UseVisualStyleBackColor = true;
			this.StartConvButton.Click += new System.EventHandler(this.StartConvButton_Click);
			// 
			// ConvertStart
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.StartConvButton);
			this.Name = "ConvertStart";
			this.Size = new System.Drawing.Size(87, 30);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button StartConvButton;
    }
}
