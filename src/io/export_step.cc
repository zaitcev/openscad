/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2023-2024 Pete Zaitcev <zaitcev@yahoo.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "export.h"

void export_step(const AbstractNode& node, std::ostream& output)
{
  // XXX
  // assert(this->root_node);
  // assert(node != nullptr);
  // assert(node != 0);

    output << "ISO-10303-21;\n";
    output << "HEADER;\n";
    output << "/* STEP */" << "\n";
    output << "ENDSEC;\n";
    output << "DATA;\n";
    output << "ENDSEC;\n";
    output << "END-ISO-10303-21;\n";

}
