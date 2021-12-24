#pragma once

// CShadowPackConfig 命令目标

class CShadowPackConfig : public CObject
{
public:
	CShadowPackConfig();
	virtual ~CShadowPackConfig();
public:
	BOOL LoadConfig();
};


