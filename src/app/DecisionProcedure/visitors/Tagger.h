//
// Created by Raph on 27/01/2016.
//

#ifndef WSKS_TAGGER_H
#define WSKS_TAGGER_H

#include "../../Frontend/ast.h"
#include "../../Frontend/ast_visitor.h"

class Tagger : public VoidVisitor{
private:
    using TagList = std::list<size_t>;
    using TagIterator = std::list<size_t>::iterator;
    size_t _lastTag = 1;
    TagList& _tagList;
    TagIterator _tit;

    void _tagFormula(ASTForm*);
public:
    Tagger(TagList& tags) : VoidVisitor(Traverse::PreOrder), _tagList(tags), _tit(tags.begin()) {}

    void visit(ASTForm_And*);
    void visit(ASTForm_Or*);
    void visit(ASTForm_Impl*);
    void visit(ASTForm_Biimpl*);

    void visit(ASTForm_Not*);
    void visit(ASTForm_Ex1*);
    void visit(ASTForm_Ex2*);
    void visit(ASTForm_All1*);
    void visit(ASTForm_All2*);

};


#endif //WSKS_TAGGER_H
