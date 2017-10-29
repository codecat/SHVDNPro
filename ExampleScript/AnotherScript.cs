using GTA;
using GTA.Native;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ExampleScript
{
	public class AnotherScript : Script
	{
		public override void OnTick()
		{
			Function.Call(Hash.DRAW_RECT, 0.3f, 0.2f, 0.1f, 0.1f, 0, 255, 0, 255);
		}
	}
}
