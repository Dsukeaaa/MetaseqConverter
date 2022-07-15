using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ResEditor2
{
	public partial class ErrorWindow : Form
	{
		#region 外部公開関数
		public void SetErrorMes(List<string> mes)
		{
			ErrorTextBox.Clear();
			foreach(string tex in mes )
			{
				ErrorTextBox.Text += tex + "\r\n";
			}
		}
		#endregion

		public ErrorWindow()
		{
			InitializeComponent();
		}

		/// <summary>
		/// OKボタン押したときの処理
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void okButton_Click(object sender, EventArgs e)
		{
			this.Dispose();
		}
	}
}
