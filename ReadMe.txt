================================================================================
    MICROSOFT 基础类库 : MultiClipBoard 项目概述
===============================================================================

应用程序向导已为您创建了此 MultiClipBoard 应用程序。此应用程序不仅演示 Microsoft 基础类的基本使用方法，还可作为您编写应用程序的起点。

本文件概要介绍组成 MultiClipBoard 应用程序的每个文件的内容。

MultiClipBoard.vcxproj
    这是使用应用程序向导生成的 VC++ 项目的主项目文件，其中包含生成该文件的 Visual C++ 的版本信息，以及有关使用应用程序向导选择的平台、配置和项目功能的信息。

MultiClipBoard.vcxproj.filters
    这是使用“应用程序向导”生成的 VC++ 项目筛选器文件。它包含有关项目文件与筛选器之间的关联信息。在 IDE 中，通过这种关联，在特定节点下以分组形式显示具有相似扩展名的文件。例如，“.cpp”文件与“源文件”筛选器关联。

MultiClipBoard.h
    这是应用程序的主头文件。
    其中包括其他项目特定的标头（包括 Resource.h），并声明 CMultiClipBoardApp 应用程序类。

MultiClipBoard.cpp
    这是包含应用程序类 CMultiClipBoardApp 的主应用程序源文件。

MultiClipBoard.rc
    这是程序使用的所有 Microsoft Windows 资源的列表。它包括 RES 子目录中存储的图标、位图和光标。此文件可以直接在 Microsoft Visual C++ 中进行编辑。项目资源包含在 2052 中。

res\MultiClipBoard.ico
    这是用作应用程序图标的图标文件。此图标包括在主资源文件 MultiClipBoard.rc 中。

res\MultiClipBoard.rc2
    此文件包含不在 Microsoft Visual C++ 中进行编辑的资源。您应该将不可由资源编辑器编辑的所有资源放在此文件中。


/////////////////////////////////////////////////////////////////////////////

应用程序向导创建一个对话框类：

MultiClipBoardDlg.h、MultiClipBoardDlg.cpp - 对话框
    这些文件包含 CMultiClipBoardDlg 类。此类定义应用程序的主对话框的行为。对话框模板包含在 MultiClipBoard.rc 中，该文件可以在 Microsoft Visual C++ 中编辑。

/////////////////////////////////////////////////////////////////////////////

其他功能：

ActiveX 控件
    该应用程序包含对使用 ActiveX 控件的支持。

打印和打印预览支持
    应用程序向导通过从 MFC 库调用 CView 类中的成员函数生成代码，来处理打印、打印设置和打印预览命令。

/////////////////////////////////////////////////////////////////////////////

其他标准文件:

StdAfx.h, StdAfx.cpp
    这些文件用于生成名为 MultiClipBoard.pch 的预编译头 (PCH) 文件和名为 StdAfx.obj 的预编译类型文件。

Resource.h
    这是标准头文件，可用于定义新的资源 ID。Microsoft Visual C++ 将读取并更新此文件。

MultiClipBoard.manifest
	Windows XP 使用应用程序清单文件来描述特定版本的并行程序集的应用程序依赖项。加载程序使用这些信息来从程序集缓存中加载相应的程序集，并保护其不被应用程序访问。应用程序清单可能会包含在内，以作为与应用程序可执行文件安装在同一文件夹中的外部 .manifest 文件进行重新分发，它还可能以资源的形式包含在可执行文件中。
/////////////////////////////////////////////////////////////////////////////

其他注释:

剪切板格式：
	1.标准的
	2.注册的
	3.私有的

	相关函数：
		RegisterClipboardFormat		注册剪切板格式
		CountClipboardFormats		统计剪切板有多少中格式
		EnumClipboardFormats		枚举剪切板格式
		GetClipboardFormatName		获取剪切板格式名

	标准剪切板格式：
		CF_BITMAP					位图句柄(HBITMAP)						需要处理
		CF_DIB						BITMAPINFO 结构体信息					需要处理
		CF_DIBV5					BITMAPV5HEADER 结构体信息				需要处理
		CF_DIF						艺术软件数据交换格式
		CF_DSPBITMAP				私有的位图格式
		CF_DSPENHMETAFILE			私有的增强图元文件格式
		CF_DSPMETAFILEPICT			私有的图元图片格式
		CF_DSPTEXT					私有的文本显示格式
		CF_ENHMETAFILE				增强图元文件句柄 (HENHMETAFILE)			需要处理
		CF_GDIOBJFIRST				应用程序自定义GDI对象值的开始
		CF_GDIOBJLAST				应用程序自定义GDI对象值的结束
		CF_HDROP					拖拽文件句柄 (HDROP)					需要处理
		CF_LOCALE					文本格式的编码标识						需要处理
		CF_METAFILEPICT				图元图片的句柄							需要处理
		CF_OEMTEXT					OEM 格式文本							需要处理
		CF_OWNERDISPLAY				剪切板自己的显示格式
		CF_PALETTE					调色板句柄
		CF_PRIVATEFIRST				微软使用的画笔扩展数据
		CF_PRIVATELAST				自定义私有剪切板格式值的开始
		CF_PRIVATEFIRST				自定义私有剪切板格式值的结束
		CF_RIFF						比波形音频更复杂的音频数据				需要处理
		CF_SYLK						微软符号链接格式						需要处理
		CF_TEXT						文本格式 换行为回车换行 ANSI格式文本	需要处理
		CF_TIFF						标签图片文件							需要处理
		CF_UNICODETEXT				文本格式 换行为回车换行 Unicode格式文本	需要处理
		CF_WAVE						标准的音频数据							需要处理


/////////////////////////////////////////////////////////////////////////////
