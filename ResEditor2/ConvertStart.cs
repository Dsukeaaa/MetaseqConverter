using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ResEditor2
{
    public partial class ConvertStart : UserControl
	{
		#region 外部公開関数
		public void OnConvert(string folderPath)
		{
			List<string> errorMes = new List<string>();
			DirectoryInfo di = new DirectoryInfo(folderPath);
			FileInfo[] files = di.GetFiles();
			foreach (FileInfo file in files)
			{
				ConvertTool tool = Util.GetToolNameByFileName(file.FullName);
				tool.SrcDataPath = file.FullName;
				ConvertToolResult result = tool.Exe();
				if (!result.IsSuccess)
				{
					errorMes.Add(result.ErrorMes);
				}
			}

			if( 0 < errorMes.Count)
			{
				MainForm mainForm = (MainForm)this.Parent;
				mainForm.OnErrorMes(errorMes);
			}
		}
		#endregion

		public ConvertStart()
        {
            InitializeComponent();
        }

		private void StartConvButton_Click(object sender, EventArgs e)
		{
			MainForm mainForm = (MainForm)this.Parent;
			mainForm.OnConvert();
		}
    }
}
