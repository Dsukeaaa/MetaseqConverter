using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace ResEditor
{
    public partial class Form1 : Form
    {
		String RootDir;

        public Form1()
        {
            InitializeComponent();

			StreamReader file = new System.IO.StreamReader(@"config.txt");
			RootDir = file.ReadLine();
			file.Close();

            if (!Directory.Exists(Path.Combine(RootDir,"data")) )
            {
                Directory.CreateDirectory(Path.Combine(RootDir,"data"));
            }
            if(!Directory.Exists( Path.Combine(RootDir,"data/build")) )
            {
                Directory.CreateDirectory(Path.Combine(RootDir,"data/build"));
			}
            if (!Directory.Exists(Path.Combine(RootDir,"data/build/chr")))
            {
                Directory.CreateDirectory(Path.Combine(RootDir,"data/build/chr"));
            }
            if (!Directory.Exists(Path.Combine(RootDir,"data/build/map")))
            {
                Directory.CreateDirectory(Path.Combine(RootDir,"data/build/map"));
            }
            
            if (!Directory.Exists(Path.Combine(RootDir,"data/work")))
            {
                Directory.CreateDirectory(Path.Combine(RootDir,"data/work"));
			}
            if (!Directory.Exists(Path.Combine(RootDir,"data/work/chr")))
            {
                Directory.CreateDirectory(Path.Combine(RootDir,"data/work/chr"));
            }
            if (!Directory.Exists(Path.Combine(RootDir,"data/work/map")))
            {
                Directory.CreateDirectory(Path.Combine(RootDir,"data/work/map"));
            }

			TreeNode root = new TreeNode("data");
			root.ImageIndex = 0;
			root.SelectedImageIndex = 0;
			_ConstructTree(Path.Combine(RootDir,"data"), ref root);
            resView.Nodes.Add(root);
        }

		private void _ConstructTree( string dirName, ref TreeNode node )
		{
			DirectoryInfo directory = new DirectoryInfo(dirName);
			// Files
			FileInfo[] files = directory.GetFiles();
			
			foreach (FileInfo file in files)
			{
				TreeNode newNode = node.Nodes.Add(file.Name);
				newNode.ImageIndex = 1;
				newNode.SelectedImageIndex = 1;
			}
			
			// Directories
			DirectoryInfo[] subDirectories = directory.GetDirectories();
			
			foreach (DirectoryInfo subDirectory in subDirectories)
			{
				TreeNode newNode = node.Nodes.Add(subDirectory.Name);
				newNode.ImageIndex = 0;
				newNode.SelectedImageIndex = 0;
				_ConstructTree(dirName+"/"+subDirectory.Name, ref newNode);
			}
		}


        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
			string path = Path.Combine(RootDir,e.Node.FullPath);
			if (File.Exists(path))
			{
				if(".txt" == Path.GetExtension(path))
				{
					using (StreamReader sr = new StreamReader(path, Encoding.GetEncoding("Shift_JIS")))
					{
						string text = sr.ReadToEnd();
						//preview.Items.Add(text);
						previewLabel.Text = text;
					}
				}
			}
			
        }

		private void ConvertButton_Click(object sender, EventArgs e)
		{
			ConvertProgress.Text = "変換中";
			TreeNode node = resView.SelectedNode;
			List<string> buildList = new List<string>();
			_Convert(node, ref buildList);

			System.Text.Encoding enc = System.Text.Encoding.GetEncoding("shift_jis");
			File.WriteAllLines(Path.Combine(RootDir,"data/build/dataList.txt"), buildList, enc);
			ConvertProgress.Text = "変換終了";

		}

		private void _Convert(TreeNode node, ref List<string> buildList)
		{
			string fileName = Path.Combine(RootDir,node.FullPath);
			if (File.Exists(fileName))
			{
				string ext = Path.GetExtension(fileName);
				string toolName = _GetTool(ext);
				if (0 < toolName.Length)
				{
					string workPath = fileName;
					string buildPath = _GetBuildPath(workPath, ext);
					Process.Start(toolName, workPath + " " + buildPath);
					buildList.Add(buildPath);
				}
			}

			foreach(TreeNode child in node.Nodes)
			{
				_Convert(child, ref buildList);
			}
		}

		//拡張子からツールの名前を取得
		private string _GetTool(string ext)
		{
			if(".fbx" == ext)
			{
				return "MetaseqConverter.exe";
			}
			return "";
		}

		//変換元から変換先を求める
		private string _GetBuildPath(string workPath, string ext)
		{
			string ret = workPath.Replace("data\\work", "data\\build");
			if(".fbx"== ext )
			{
				ret = ret.Replace(".fbx", ".dmdl");
			}
			return ret;
		}

		private void resView_MouseDown(object sender, MouseEventArgs e)
		{
			// 右クリックでもノードを選択させる
			if (e.Button == MouseButtons.Right)
			{
				resView.SelectedNode = resView.GetNodeAt(e.X, e.Y);
			}
		}

		private void ExplolerToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if(null != resView.SelectedNode)
			{
				string path = resView.SelectedNode.FullPath;
				string fullPath = Path.Combine(RootDir, path);
				Process.Start("EXPLORER.EXE", fullPath);
			}
		}
    }
}
