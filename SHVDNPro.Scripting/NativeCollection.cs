using GTA.Native;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// This is only here for compatibility with GTMP.
// It's literally the only reason.
// You really don't have to use this.
// Seriously, don't bother.

namespace GTA.Tasks
{
	public class NativeCollectionItem
	{
		private Hash m_hash;
		private InputArgument[] m_args;

		public NativeCollectionItem(Hash hash, params InputArgument[] args)
		{
			m_hash = hash;
			m_args = args;
		}

		internal void Run()
		{
			Function.Call(m_hash, m_args);
		}
	}

	public class NativeCollection
	{
		private List<NativeCollectionItem> m_items = new List<NativeCollectionItem>();

		public void Add(NativeCollectionItem item)
		{
			m_items.Add(item);
		}

		public void Add(Hash hash, params InputArgument[] args)
		{
			m_items.Add(new NativeCollectionItem(hash, args));
		}

		public void Run()
		{
			foreach (var item in m_items) {
				item.Run();
			}
		}
	}
}
