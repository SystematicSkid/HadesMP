#pragma once

namespace engine::hades
{
	class InputHandler
	{
	public:
		std::vector<int> GetButtons()
		{
			uintptr_t begin = *(uintptr_t*)((uintptr_t)this + 0x70);
			uintptr_t end = *(uintptr_t*)((uintptr_t)this + 0x78);
			int num_buttons = (end - begin) / 0x8;

			std::vector<int> ret;
			int* buttons = *(int**)((uintptr_t)this + 0x70);
			for (int i = 0; i < num_buttons; i++)
				ret.push_back(buttons[i]);
			return ret;
		}

		void SetButtons(std::vector<int> new_input)
		{
			int* buttons = *(int**)((uintptr_t)this + 0x70);
			for (int i = 0; i < new_input.size(); i++)
			{
				buttons[i] = new_input[i];
			}
		}
	};
}