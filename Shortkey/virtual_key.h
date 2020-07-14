#pragma once

struct sVirtualKey {
	DWORD vk			: 8;
	DWORD VK_KEY_UP		: 1;
	DWORD VK_KEY_CTRL	: 1;
	DWORD VK_KEY_ALT	: 1;
	DWORD VK_KEY_SHIFT	: 1;
};

struct sVirtualKeyDesc {
	int vk;
	const char *name;
	const char *desc;
};

extern sVirtualKeyDesc g_vk_list[];

extern int getVkIndex (int vk);
extern int GetVirtualKeyCode (int index);
extern const char *GetVirtualKeyDesc (int index);
extern const char *GetVirtualKeyDescFromVK (int vk);
extern const char *GetVirtualKeyNameFromVK (int vk);
