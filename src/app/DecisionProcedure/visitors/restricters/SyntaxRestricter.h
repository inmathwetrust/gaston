/*****************************************************************************
 *  dWiNA - Deciding WSkS using non-deterministic automata
 *
 *  Copyright (c) 2015  Tomas Fiedor <xfiedo01@stud.fit.vutbr.cz>
 *
 *  Description:
 *    Visitor for restricting the syntax of the WSkS logic. Restricts the
 *    logical connectives to or, and, not only.
 *
 *****************************************************************************/

#ifndef WSKS_SYNTAXRESTRICTER_H
#define WSKS_SYNTAXRESTRICTER_H

#include "../../../Frontend/ast.h"
#include "../../../Frontend/ast_visitor.h"

class SyntaxRestricter : public TransformerVisitor {
public:
    SyntaxRestricter() : TransformerVisitor(Traverse::PostOrder) {}
    AST* visit(ASTForm_Impl* form);
    AST* visit(ASTForm_Biimpl* form);
    AST* visit(ASTForm_Export* form);
};


#endif //WSKS_SYNTAXRESTRICTER_H
