using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace ResEditor2
{
	class ConvertToolResult
	{
		public bool IsSuccess{ get; set;}
		public string ErrorMes { get; set; }
		public ConvertToolResult()
		{
			IsSuccess = false;
			ErrorMes = "";
		}
	}

    class ConvertTool
    {
        public String SrcDataPath { get; set; }
        public string ToolName { get; protected set; }
		public virtual ConvertToolResult Exe() {
			ConvertToolResult ret = new ConvertToolResult();
			ret.IsSuccess = true;//存在するファイル全てに実行されるのでここに来るのは正常
			return ret;
		}
    }

	/// <summary>
	/// fbxを変換
	/// Arg1：元データパス
	/// Arg2：出力データパス
	/// </summary>
	class MetaseqConverter : ConvertTool
    {
        public MetaseqConverter(string name)
        {
			ToolName = name;
        }
		public override ConvertToolResult Exe() 
		{
			ConvertToolResult ret = new ConvertToolResult();
			if (File.Exists(ToolName))
			{
				string distPath = SrcDataPath.Replace(".fbx", ".dmdl");
				Process.Start(ToolName, SrcDataPath + " " + distPath);
				ret.IsSuccess = true;
			}
			else
			{
				ret.IsSuccess = false;
				ret.ErrorMes = "変換ツール" + ToolName + "が存在しません";
			}
			return ret;
		}
    }
}
