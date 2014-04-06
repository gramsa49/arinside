//Copyright (C) 2014 John Luthgers | jls17
//
//This file is part of ARInside.
//
//    ARInside is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    ARInside is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "../core/ARServerObject.h"
#include "../core/ARContainer.h"
#include "ImageTag.h"
#include "RootPath.h"
#include "URLLink.h"
#include "WebUtil.h"

namespace OUTPUT
{
	ImageTag::ImageEnum GetContainerImage(unsigned int containerType)
	{
		switch (containerType)
		{
		case ARCON_GUIDE: return ImageTag::ActiveLinkGuide;
		case ARCON_APP: return ImageTag::Application;
		case ARCON_PACK: return ImageTag::PackingList;
		case ARCON_FILTER_GUIDE: return ImageTag::FilterGuide;
		case ARCON_WEBSERVICE: return ImageTag::Webservice;
		default: return ImageTag::Document;
		}
	}

	ImageTag::ImageEnum MapXmlStructItemToImage(const CARServerObject &serverObj)
	{
		switch (serverObj.GetServerObjectTypeXML())
		{
		case AR_STRUCT_ITEM_XML_ACTIVE_LINK: return ImageTag::ActiveLink;
		case AR_STRUCT_ITEM_XML_FILTER: return ImageTag::Filter;
		case AR_STRUCT_ITEM_XML_ESCALATION: return ImageTag::Escalation;
		case AR_STRUCT_ITEM_XML_CHAR_MENU: return ImageTag::Menu;
		case AR_STRUCT_ITEM_XML_CONTAINER:
			{
				const CARContainer &cnt = dynamic_cast<const CARContainer&>(serverObj);
				return GetContainerImage(cnt.GetType());
			}
			break;
		case AR_STRUCT_ITEM_XML_IMAGE: return ImageTag::Image;
		default: return ImageTag::NoImage;
		}
	}

	// constructors
	URLLink::URLLink(const std::string &caption, const CPageParams &linkToPage, const OUTPUT::ImageTag &image, int rootLevel, bool validate, OUTPUT::URLLink::LinkTargetMode target)
	{
		Init(caption, linkToPage, image, rootLevel, validate, target);
	}

	URLLink::URLLink(const std::string &caption, const CPageParams &linkToPage, const OUTPUT::ImageTag &image, int rootLevel, OUTPUT::URLLink::LinkTargetMode target)
	{
		Init(caption, linkToPage, image, rootLevel, true, target);
	}
	URLLink::URLLink(const CARServerObject &workflowObject, int rootLevel, bool showImage)
	{
		CPageParams linkToPage(PAGE_DETAILS, &workflowObject);
		ImageTag image(showImage ? MapXmlStructItemToImage(workflowObject) : ImageTag::NoImage, rootLevel);
		Init(workflowObject.GetName(), linkToPage, image, rootLevel, true, URLLink::TARGET_MODE_SELF);
	}

	// Central class initialization
	void URLLink::Init(const std::string &caption, const CPageParams &linkToPage, const OUTPUT::ImageTag &image, int rootLevel, bool validate, OUTPUT::URLLink::LinkTargetMode target)
	{
		stringstream strmTmp;

		strmTmp << image;
		
		if (!linkToPage.Valid())
			AddCaption(strmTmp, caption, validate);
		else
		{
			strmTmp << "<a href=\"" << RootPath(rootLevel) << linkToPage->GetFullFileName() << "\"";
			if (target > TARGET_MODE_SELF)
			{
				strmTmp << " target=\"";
				switch (target)
				{
					case TARGET_MODE_PARENT: strmTmp << "_parent"; break;
					case TARGET_MODE_TOP: strmTmp << "_top"; break;
					case TARGET_MODE_BLANK: strmTmp << "_blank"; break;
				}
				strmTmp << "\"";
			}
			strmTmp << ">"; 
			AddCaption(strmTmp, caption, validate);
			strmTmp << "</a>";
		}

		link = strmTmp.str();
	}

	void URLLink::AddCaption(std::ostream &strm, const std::string &caption, bool validate)
	{
		strm << (validate ? CWebUtil::Validate(caption) : caption);
	}

	ostream& URLLink::ToStream(std::ostream &strm) const
	{
		strm << link;
		return strm;
	}

	ostream& operator <<(ostream &strm, const URLLink &link)
	{
		return link.ToStream(strm);
	}
}; // end namespace OUTPUT
