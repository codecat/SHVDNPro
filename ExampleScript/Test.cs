using GTA;
using GTA.Native;
using GTA.Math;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExampleScript
{
	public class Test : Script
	{
		internal static StreamWriter m_log;
		private DateTime m_lastTick;

		public Test()
		{
			m_log = new StreamWriter("ExampleScript.log", true);
			m_log.AutoFlush = true;
			m_log.WriteLine("ExampleScript says hi from ctor at {0}", DateTime.Now);

			m_lastTick = DateTime.Now;
		}

		public override void OnTick()
		{
			if ((DateTime.Now - m_lastTick).TotalSeconds < 1.0) {
				return;
			}

			m_log.WriteLine("Yielding at {0}.{1}", DateTime.Now.Second, DateTime.Now.Millisecond);
			Yield();
		}
	}
}
