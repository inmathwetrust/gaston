/*****************************************************************************
 *  dWiNA - Deciding WSkS using non-deterministic automata
 *
 *  Copyright (c) 2015  Tomas Fiedor <xfiedo01@stud.fit.vutbr.cz>
 *
 *  Description:
 *    Visitor for restricting the syntax of the WSkS logic.
 *
 *****************************************************************************/
#include "SyntaxRestricter.h"
#include "../../../Frontend/ast.h"
#include "../../environment.hh"

/**
 * Transforms implication to restricted syntax as follows:
 *  A => B = not A or B
 *
 *  @param[in] form     traversed Impl node
 */
AST* SyntaxRestricter::visit(ASTForm_Impl* form) {
    assert(form != nullptr);

    // not f1
    ASTForm_Not* notF = new ASTForm_Not(form->f1, form->f1->pos);
    // not f1 or f2
    ASTForm_Or* notForFF = new ASTForm_Or(notF, form->f2, form->pos);

    // Delete the implication node
    form->detach();
    delete form;

    return notForFF;
}

/**
 * Transforms biimplication to two implications:
 *  A <=> B = A => B and B => A
 *
 * @param[in] form      traversed Biimpl node
 */
AST* SyntaxRestricter::visit(ASTForm_Biimpl* form) {
#   if (OPT_BI_AND_IMPLICATION_SUPPORT == true)
    return form;
#   else
    assert(form != nullptr);

    ASTForm_Not* notF = new ASTForm_Not(form->f1, form->f1->pos);
    ASTForm_Not* notFF = new ASTForm_Not(form->f2, form->f2->pos);

    ASTForm* ff1 = form->f1->clone();
    ASTForm* ff2 = form->f2->clone();

    ASTForm* impl1 = new ASTForm_Or(notF, ff2, form->pos);
    ASTForm* impl2 = new ASTForm_Or(ff1, notFF, form->pos);

    ASTForm_And* biimpl = new ASTForm_And(impl1, impl2, form->pos);

    // Delete the Biimpl node
    form->detach();
    delete form;

    return biimpl;
#   endif
}

AST* SyntaxRestricter::visit(ASTForm_Export* form) {
    assert(form != nullptr);
    std::cout << "[!] \033[1;31mWarning\033[0m: Export skipped (not supported)\n";
    return form->f;
}