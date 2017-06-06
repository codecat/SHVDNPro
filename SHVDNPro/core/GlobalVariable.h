#pragma once

namespace GTA
{
	namespace Native
	{
		public value class GlobalVariable sealed
		{
		public:
			/// <summary>
			/// Gets the global variable at the specified index.
			/// </summary>
			/// <param name="index">The index of the global variable.</param>
			/// <returns>A <see cref="GlobalVariable"/> instance representing the global variable.</returns>
			static GlobalVariable Get(int index);

			/// <summary>
			/// Gets the native memory address of the <see cref="GlobalVariable"/>.
			/// </summary>
			property System::IntPtr MemoryAddress
			{
				System::IntPtr get()
				{
					return _address;
				}
			}

			/// <summary>
			/// Gets the value stored in the <see cref="GlobalVariable"/>.
			/// </summary>
			generic <typename T> T Read();
			/// <summary>
			/// Set the value stored in the <see cref="GlobalVariable"/>.
			/// </summary>
			/// <param name="value">The new value to assign to the <see cref="GlobalVariable"/>.</param>
			generic <typename T> void Write(T value);
			/// <summary>
			/// Set the value stored in the <see cref="GlobalVariable"/> to a string.
			/// </summary>
			/// <param name="value">The string to set the <see cref="GlobalVariable"/> to.</param>
			/// <param name="maxSize">The maximum size of the string. Can be found for a given global variable by checking the decompiled scripts from the game.</param>
			void WriteString(System::String ^value, int maxSize);

			/// <summary>
			/// Set the value of a specific bit of the <see cref="GlobalVariable"/> to true.
			/// </summary>
			/// <param name="index">The zero indexed bit of the <see cref="GlobalVariable"/> to set.</param>
			void SetBit(int index);
			/// <summary>
			/// Set the value of a specific bit of the <see cref="GlobalVariable"/> to false.
			/// </summary>
			/// <param name="index">The zero indexed bit of the <see cref="GlobalVariable"/> to clear.</param>
			void ClearBit(int index);
			/// <summary>
			/// Gets a value indicating whether a specific bit of the <see cref="GlobalVariable"/> is set.
			/// </summary>
			/// <param name="index">The zero indexed bit of the <see cref="GlobalVariable"/> to check.</param>
			bool IsBitSet(int index);

			/// <summary>
			/// Gets the <see cref="GlobalVariable"/> stored at a given offset in a global structure.
			/// </summary>
			/// <param name="index">The index the <see cref="GlobalVariable"/> is stored in the structure. For example the Y component of a Vector3 is at index 1.</param>
			/// <returns>The <see cref="GlobalVariable"/> at the index given.</returns>
			GlobalVariable GetStructField(int index);

			/// <summary>
			/// Returns an array of all <see cref="GlobalVariable"/>s in a global array.
			/// </summary>
			/// <param name="itemSize">The number of items stored in each array index. For example an array of Vector3s takes up 3 items.</param>
			/// <returns>The array of <see cref="GlobalVariable"/>s.</returns>
			array<GlobalVariable> ^GetArray(int itemSize);
			/// <summary>
			/// Gets the <see cref="GlobalVariable"/> stored at a specific index in a global array.
			/// </summary>
			/// <param name="index">The array index.</param>
			/// <param name="itemSize">The number of items stored in each array index. For example an array of Vector3s takes up 3 items.</param>
			/// <returns>The <see cref="GlobalVariable"/> at the index given.</returns>
			GlobalVariable GetArrayItem(int index, int itemSize);

		private:
			GlobalVariable(System::IntPtr address);

			System::IntPtr _address;
		};
	}
}
