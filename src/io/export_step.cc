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

// #include <string>
#include <memory>
#include <ostream>
#include "NodeDumper.h"   // XXX Needed for the GroupNodeChecker
#include "export.h"
#include "NodeVisitor.h"

class NodeStepper : public NodeVisitor
{
public:
  NodeStepper(std::shared_ptr<const AbstractNode> root_node, std::string indent, std::ostream& output) :
    indent(std::move(indent)), root(std::move(root_node)), dumpstream(output) {}

  Response visit(State& state, const AbstractNode& node) override;
  Response visit(State& state, const GroupNode& node) override;
  Response visit(State& state, const ListNode& node) override;
  Response visit(State& state, const RootNode& node) override;

private:
#if 0  /* XXX */
  void initCache();
  void finalizeCache();
  bool isCached(const AbstractNode& node) const;
#endif

  // Output Formatting options
  std::string indent;
  bool idString;

  int currindent{0};
  std::shared_ptr<const AbstractNode> root;
  GroupNodeChecker groupChecker;
  // std::ostringstream dumpstream;
  std::ostream& dumpstream;
};

/*
 * Called for each node in the tree.
 */
Response NodeStepper::visit(State& state, const AbstractNode& node)
{
  if (state.isPrefix()) {

#if 0  /* XXX */
    // For handling root modifier '!'
    // Check if we are processing the root of the current Tree and init cache
    if (this->root.get() == &node) {
      this->initCache();
    }

    // ListNodes can pass down modifiers to children via state, so check both modinst and state
    if (node.modinst->isBackground() || state.isBackground()) this->dumpstream << "%";
    if (node.modinst->isHighlight() || state.isHighlight()) this->dumpstream << "#";
#endif

    this->dumpstream << "; AbstractNode isPrefix idx" << node.index() << "\n";

    if (this->idString) {

      if (node.getChildren().size() > 0) {
        this->dumpstream << "{";
      }

    } else {

      for (int i = 0; i < this->currindent; ++i) {
        this->dumpstream << this->indent;
      }
      this->dumpstream << node;   // XXX Change to STEP
      this->dumpstream << "\n";
      if (node.getChildren().size() > 0) {
        this->dumpstream << " {\n";
      }
    }

    this->currindent++;

  } else if (state.isPostfix()) {

    this->dumpstream << "; AbstractNode isPostfix\n";

    this->currindent--;

    if (this->idString) {
      if (node.getChildren().size() > 0) {
        this->dumpstream << "}";
      } else {
        this->dumpstream << ";";
      }
    } else {
      if (node.getChildren().size() > 0) {
        for (int i = 0; i < this->currindent; ++i) {
          this->dumpstream << this->indent;
        }
        this->dumpstream << "}\n";
      } else {
        this->dumpstream << ";\n";
      }
    }

#if 0  /* XXX */
    // For handling root modifier '!'
    // Check if we are processing the root of the current Tree and finalize cache
    if (this->root.get() == &node) {
      this->finalizeCache();
    }
#endif
  } else {
    this->dumpstream << "; AbstractNode else\n";
  }

  return Response::ContinueTraversal;
}

Response NodeStepper::visit(State& state, const GroupNode& node)
{
  // XXX
  if (!this->idString) {
    this->dumpstream << "; GroupNode id force AbstractNode\n";
    return NodeStepper::visit(state, (const AbstractNode&)node);
  }

  // This chunk is copied from NodeDumper verbatim.
  this->dumpstream << "; GroupNode\n";
  if (state.isPrefix()) {
    if (this->groupChecker.getChildCount(node.index()) > 1) {
      this->dumpstream << node << "{";
    }
    this->currindent++;
  } else if (state.isPostfix()) {
    this->currindent--;
    if (this->groupChecker.getChildCount(node.index()) > 1) {
      this->dumpstream << "}";
    }
  }

  return Response::ContinueTraversal;
}

/*!
   Handle list nodes specially: Only list children
 */
Response NodeStepper::visit(State& state, const ListNode& node)
{
  this->dumpstream << "; ListNode\n";
  if (state.isPrefix()) {
    // pass modifiers down to children via state
    if (node.modinst->isHighlight()) state.setHighlight(true);
    if (node.modinst->isBackground()) state.setBackground(true);
  } else if (state.isPostfix()) {
    ;
  }
  return Response::ContinueTraversal;
}

/*!
   Handle root nodes specially: Only list children
 */
Response NodeStepper::visit(State& state, const RootNode& node)
{
#if 0  /* XXX */
  if (isCached(node)) return Response::PruneTraversal;
#endif
  this->dumpstream << "; RootNode\n";
  return Response::ContinueTraversal;
}

void export_step(const std::shared_ptr<const AbstractNode>& node, std::ostream& output)
{

    output << "ISO-10303-21;\n";
    output << "HEADER;\n";
    output << "FILE_DESCRIPTION(('OpenCAD Model'),'2;1');\n";
    output << "FILE_NAME('Open CASCADE Shape Model','2023-12-28T14:28:00',(\n";
    // XXX Find a way to include the version automatically.
    output << "    'Unknown Author'),(''),'OpenSCAD 2021.01','OpenSCAD','Not Applicable');\n";
    output << "FILE_SCHEMA(('AUTOMOTIVE_DESIGN { 1 0 10303 214 1 1 1 1 }'));\n";
    output << "ENDSEC;\n";
    output << "DATA;\n";

    std::string indent("> ");
    // XXX why is root_node used twice
    // NodeStepper stepper(this->root_node, indent, idString);
    // stepper.traverse(*this->root_node);
    NodeStepper stepper(node, indent, output);
    stepper.traverse(*node);

    output << "ENDSEC;\n";
    output << "END-ISO-10303-21;\n";

}
