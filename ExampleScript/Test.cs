using GTA;
using GTA.Native;
using System;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ExampleScript
{
	public class Test : Script
	{
		internal static StreamWriter m_log;
		private DateTime m_lastTick;

		public override void OnInit()
		{
			m_log = new StreamWriter("ExampleScript.log", true);
			m_log.AutoFlush = true;
			m_log.WriteLine("ExampleScript says hi from ctor at {0}", DateTime.Now);

			m_lastTick = DateTime.Now;

			int player = Function.Call<int>(Hash.GET_PLAYER_INDEX);
			m_log.WriteLine("Player index in OnInit: {0}", player);
		}

		public override void OnTick()
		{
			Function.Call(Hash.DRAW_RECT, 0.0f, 0.0f, 0.1f, 0.1f, 255, 0, 0, 255);
		}

		public override void OnKeyDown(KeyEventArgs args)
		{
			if (args.KeyCode == Keys.J) {
				m_log.WriteLine("You pressed J at {0}!", DateTime.Now);

			} else if (args.KeyCode == Keys.K) {
				m_log.WriteLine("You pressed K, you die!");
				Game.PlayerPed.Kill();
			}
		}

		public override void OnKeyUp(KeyEventArgs args)
		{
			if (args.KeyCode == Keys.J) {
				m_log.WriteLine("You released J at {0}!", DateTime.Now);
			}
		}

		public override void OnPresent(IntPtr swapchain)
		{
		}
	}
}
