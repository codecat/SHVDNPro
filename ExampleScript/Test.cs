using GTA;
using GTA.Native;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ExampleScript
{
	public class Test : Script
	{
		internal static StreamWriter m_log;

		public override void OnInit()
		{
			m_log = new StreamWriter("ExampleScript.log", true);
			m_log.AutoFlush = true;
			m_log.WriteLine("ExampleScript says hi from ctor at {0}", DateTime.Now);
		}

		public override void OnTick()
		{
			Function.Call(Hash.DRAW_RECT, 0.1f, 0.2f, 0.1f, 0.1f, 255, 0, 0, 255);
		}
	}
}
