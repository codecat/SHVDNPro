using GTA.Math;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GTA.Math
{
	// For natives that require pointers to vectors and are called internally in the scripting section.
	[StructLayout(LayoutKind.Explicit, Size = 0x18)]
	internal struct NativeVector3
	{
		[FieldOffset(0x00)]
		internal float X;
		[FieldOffset(0x08)]
		internal float Y;
		[FieldOffset(0x10)]
		internal float Z;

		internal NativeVector3(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		public static implicit operator Vector3(NativeVector3 value) => new Vector3(value.X, value.Y, value.Z);

		public static implicit operator NativeVector3(Vector3 value) => new NativeVector3(value.X, value.Y, value.Z);
	}
}
