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
    public partial class MainForm : Form
	{
		#region 外部公開関数
		public void OnConvert()
		{
			convertStart1.OnConvert(folderView1.GetFolderPath());
		}

		public void OnErrorMes(List<string> mes)
		{
			ErrorWindow error = new ErrorWindow();
			error.SetErrorMes(mes);
			error.Show();
			
		}
		#endregion

		public MainForm()
        {
            InitializeComponent();
		}
    }
}
