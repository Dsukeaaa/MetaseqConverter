using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Management; 


namespace ResEditor2
{
	public partial class FolderView : UserControl
	{
		#region 外部公開関数
		public string GetFolderPath(){ return m_currentFolderPath; }
		#endregion


		#region メンバ
		private List<ListViewItemHistory> m_history;
		private int m_historyIdx;
		private int m_historyMaxIdx;
		private string m_currentFolderPath;
		#endregion

		private class ListViewItemTag
		{
			public string FullPath { set; get; }
		}

		private class ListViewItemHistory
		{
			public List<ListViewItem> Items { get; set; }
			public string FullPath { get; set; }
			public ListViewItemHistory()
			{
				Items = new List<ListViewItem>();
				FullPath = "";
			}
		}

		public FolderView()
		{
			m_history = new List<ListViewItemHistory>();
			m_historyIdx = -1;
			m_historyMaxIdx = -1;
			InitializeComponent();

			{//List に一覧を登録
				string path = System.Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
				m_currentFolderPath = path;
				DirectoryInfo di = new DirectoryInfo(path);     //パスを指定
				
				//ディレクトリ一覧を登録
				DirectoryInfo[] dis = di.GetDirectories("*.*"); //パターンを指定
				foreach (DirectoryInfo diInfo in dis)
				{
					ListViewItemTag tag = new ListViewItemTag();
					tag.FullPath = diInfo.FullName;
					ListViewItem item = new ListViewItem(diInfo.Name, 0);
					item.Tag = tag;
					listView1.Items.Add(item);
				}
				//ファイル一覧を登録
				FileInfo[] files = di.GetFiles();
				foreach (FileInfo file in files)
				{
					listView1.Items.Add(new ListViewItem(file.Name, 1));
				}

				//今の状態を履歴に
				_StoreHistory(listView1, path);
			}
			{//Tree にドライブ一覧を登録
				ManagementObject mo = new ManagementObject();
				// 論理ドライブ一覧を列挙
				string[] drives = Environment.GetLogicalDrives();
				foreach (string drive in drives){
					// ドライブのタイプを取得
					mo.Path = new ManagementPath("Win32_LogicalDisk='" + drive.TrimEnd('\\') + "'");
					int drivetype = Convert.ToInt32(mo.Properties["DriveType"].Value);
					// ドライブのタイプごとにアイコンを設定
					TreeNode node = null;
					switch (drivetype){
						case 0:
							//ドライブを判別できません
							break;
						case 1:
							//ドライブ上にルートディレクトリが存在しません
							break;
						case 2:
							//Floppy
							node = new TreeNode(drive, 4, 4);
							break;
						case 3:
						case 6:
							//HardDisk
							node = new TreeNode(drive, 0, 0);
							break;
						case 4:
							//Network
							node = new TreeNode(drive, 5, 5);
							break;
						case 5:
							//CD-ROM
							node = new TreeNode(drive, 3, 3);
							break;
					}
					if (null != node)
					{
						treeView1.Nodes.Add(node);

						// +ボックスを表示するためのダミー
						node.Nodes.Add("dummy");
					}
				}
			}
		}



		/// <summary>
		/// ツリービュー開いたときの処理
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void treeView1_BeforeExpand(object sender, TreeViewCancelEventArgs e)
		{
			TreeNode node = e.Node;

			// 展開するノードのフルパスを取得
			string fullpath = node.FullPath;
			node.Nodes.Clear();

			
			DirectoryInfo dirs = new DirectoryInfo(fullpath);
			if (dirs != null)
			{
				// フォルダ一覧を取得
				foreach (DirectoryInfo dir in dirs.GetDirectories())
				{
					// フォルダを追加
					TreeNode nodeFolder = new TreeNode(dir.Name, 0, 0);
					node.Nodes.Add(nodeFolder);

					// +ボックスを表示するためのダミー
					nodeFolder.Nodes.Add("dummy");
				}

				// ファイル一覧を取得
				FileInfo[] files = dirs.GetFiles();
				foreach (FileInfo file in files)
				{
					TreeNode nodeFile = new TreeNode(file.Name, 1, 1);
					node.Nodes.Add(nodeFile);
				}

			}
		}

		/// <summary>
		/// ツリービュー選択時
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void treeView1_BeforeSelect(object sender, TreeViewCancelEventArgs e)
		{
			TreeNode node = e.Node;
			string fullpath = node.FullPath;
			_ItemUpdate(listView1, fullpath, true);
		}

		/// <summary>
		/// ツリービューのキーダウン
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void treeView1_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyData == Keys.F5)
			{
				_ItemUpdate(listView1, m_currentFolderPath, false);
			}
		}





		/// <summary>
		/// リストビューのアイテム選択時の処理
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void listView1_ItemActivate(object sender, EventArgs e)
		{
			ListView listView = (ListView)sender;
			if (listView.SelectedItems.Count == 1)
			{
				ListViewItem selectItem = listView.SelectedItems[0];
				ListViewItemTag selectTag = (ListViewItemTag)selectItem.Tag;
				string folderPath = selectTag.FullPath;

				_ItemUpdate(listView, folderPath, true);
			}
		}

		/// <summary>
		/// リストビューのキーダウン
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void listView1_KeyDown(object sender, KeyEventArgs e)
		{
			if( e.KeyData == Keys.F5 )
			{
				_ItemUpdate(listView1, m_currentFolderPath, false);
			}
		}


		/// <summary>
		/// リストビューをfolderPathの中身に更新
		/// </summary>
		/// <param name="listView"></param>
		/// <param name="folderPath"></param>
		private void _ItemUpdate(ListView listView, string folderPath, bool isUpdateHistory)
		{
			//選択したのがフォルダーなら階層を降りる
			if (Directory.Exists(folderPath)) 
			{
				m_currentFolderPath = folderPath;
				listView.Items.Clear();

				DirectoryInfo di = new DirectoryInfo(folderPath);
				try
				{
					DirectoryInfo[] dis = di.GetDirectories("*.*"); //パターンを指定
					foreach (DirectoryInfo diInfo in dis)
					{
						ListViewItemTag addTag = new ListViewItemTag();
						addTag.FullPath = diInfo.FullName;
						ListViewItem addItem = new ListViewItem(diInfo.Name, 0);
						addItem.Tag = addTag;
						listView.Items.Add(addItem);
					}
					//ファイル一覧を登録
					FileInfo[] files = di.GetFiles();
					foreach (FileInfo file in files)
					{
						listView.Items.Add(new ListViewItem(file.Name, 1));
					}
				}catch(Exception){
					//何もフォルダがないとアクセス拒否でミスるのでキャッチ
				}

				if (isUpdateHistory)
				{
					//新しいアイテムを履歴に。
					_StoreHistory(listView, folderPath);
					m_historyMaxIdx = m_historyIdx;//クリックして選択したときはそこが履歴の末尾
				}
			}
			//進む・戻るボタンの色更新
			backButton.Refresh();
			forwardButton.Refresh();
		}

		/// <summary>
		/// 現在のアイテムを履歴に残す
		/// </summary>
		/// <param name="storeListView"></param>
		private void _StoreHistory( ListView storeListView, string fullPath)
		{
			ListViewItemHistory newHistoryItem = new ListViewItemHistory();
			foreach (ListViewItem nowItem in storeListView.Items)
			{
				newHistoryItem.Items.Add(nowItem);
				newHistoryItem.FullPath = fullPath;
			}

			++m_historyIdx;
			if (m_historyIdx < m_history.Count)
			{
				m_history[m_historyIdx] = newHistoryItem;
			}
			else
			{
				m_history.Add(newHistoryItem);
			}
			
		}

		/// <summary>
		/// 戻るボタン
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void backButton_Click(object sender, EventArgs e)
		{
			if(0 < m_historyIdx)
			{
				--m_historyIdx;
				List<ListViewItem> items = m_history[m_historyIdx].Items;
				listView1.Items.Clear();
				foreach(ListViewItem item in items)
				{
					listView1.Items.Add(item);
				}
			}
			backButton.Refresh();
			forwardButton.Refresh();
		}
		/// <summary>
		///  戻るボタン描画
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void backButton_Paint(object sender, PaintEventArgs e)
		{
			if(0 < m_historyIdx)
			{
				backButton.ForeColor = SystemColors.ControlText;
			}
			else
			{
				backButton.ForeColor = SystemColors.GradientActiveCaption;
			}
		}

		/// <summary>
		/// 進むボタン
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void forwardButton_Click(object sender, EventArgs e)
		{
			if (m_historyIdx < m_historyMaxIdx)
			{
				++m_historyIdx;
				List<ListViewItem> items = m_history[m_historyIdx].Items;
				listView1.Items.Clear();
				foreach (ListViewItem item in items)
				{
					listView1.Items.Add(item);
				}
			}

			backButton.Refresh();
			forwardButton.Refresh();
		}

		/// <summary>l
		/// 進むボタン描画
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void forwardButton_Paint(object sender, PaintEventArgs e)
		{
			if (m_historyIdx < m_historyMaxIdx)
			{
				forwardButton.ForeColor = SystemColors.ControlText;
			}
			else
			{
				forwardButton.ForeColor = SystemColors.GradientActiveCaption;
			}
		}





		/// <summary>
		/// アドレスバーのキーダウン
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void addressTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			TextBox texBox = (TextBox)sender;
			if( e.KeyData == Keys.Enter )
			{
				_ItemUpdate(listView1, texBox.Text, true);
			}
		}
	}
}
