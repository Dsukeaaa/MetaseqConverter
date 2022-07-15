using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace ResEditor2
{
    class Util
    {
        //拡張子からツールの名前を取得
		public static ConvertTool GetToolNameByFileName(string dataName)
        {
			ConvertTool ret = new ConvertTool();
			string ext = Path.GetExtension(dataName);
            if (".fbx" == ext)
            {
                ret = new MetaseqConverter("MetaseqConverter.exe");
            }
            return ret;
        }

        //エクスプローラー起動
        public void ExplolerExe(string path)
        {
            Process.Start("EXPLORER.EXE", path);
        }
    }
}
