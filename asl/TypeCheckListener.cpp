//////////////////////////////////////////////////////////////////////
//
//    TypeCheckListener - Walk the parser tree to do the semantic
//                        typecheck for the Asl programming language
//
//    Copyright (C) 2018  Universitat Politecnica de Catalunya
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public License
//    as published by the Free Software Foundation; either version 3
//    of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//    contact: José Miguel Rivero (rivero@cs.upc.edu)
//             Computer Science Department
//             Universitat Politecnica de Catalunya
//             despatx Omega.110 - Campus Nord UPC
//             08034 Barcelona.  SPAIN
//
//////////////////////////////////////////////////////////////////////

#include "TypeCheckListener.h"

#include "antlr4-runtime.h"

#include "../common/TypesMgr.h"
#include "../common/SymTable.h"
#include "../common/TreeDecoration.h"
#include "../common/SemErrors.h"

#include <iostream>
#include <string>

// uncomment the following line to enable debugging messages with DEBUG*
// #define DEBUG_BUILD
#include "../common/debug.h"

// using namespace std;


// Constructor
TypeCheckListener::TypeCheckListener(TypesMgr       & Types,
                                     SymTable       & Symbols,
                                     TreeDecoration & Decorations,
                                     SemErrors      & Errors) :
  Types{Types},
  Symbols {Symbols},
  Decorations{Decorations},
  Errors{Errors} {
}

void TypeCheckListener::enterProgram(AslParser::ProgramContext *ctx) {
  DEBUG_ENTER();

  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
}
void TypeCheckListener::exitProgram(AslParser::ProgramContext *ctx) {
  if (Symbols.noMainProperlyDeclared())
    Errors.noMainProperlyDeclared(ctx);
  Symbols.popScope();
  Errors.print();
  DEBUG_EXIT();
}

void TypeCheckListener::enterFunction(AslParser::FunctionContext *ctx) {
  DEBUG_ENTER();
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
  auto ret = getTypeDecor(ctx->type());
  std::vector<TypesMgr::TypeId> lParamsTy;
  auto paramList = ctx->parameters();
  for (auto params : paramList->parameter_decl()) {
      TypesMgr::TypeId parameterType;
      if (params->type()) parameterType = getTypeDecor(params->type());
      else if (params->array_decl()) parameterType = getTypeDecor(params->array_decl());
      for (auto ids : params->ID()) {
          lParamsTy.push_back(parameterType);
      }
  }
  auto t = Types.createFunctionTy(lParamsTy,ret);
  Symbols.setCurrentFunctionTy(t);
}

void TypeCheckListener::exitFunction(AslParser::FunctionContext *ctx) {
  Symbols.popScope();
  DEBUG_EXIT();
}

void TypeCheckListener::enterDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterVariable_decl(AslParser::Variable_declContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitVariable_decl(AslParser::Variable_declContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterType(AslParser::TypeContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitType(AslParser::TypeContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterStatements(AslParser::StatementsContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitStatements(AslParser::StatementsContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterAssignStmt(AslParser::AssignStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitAssignStmt(AslParser::AssignStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->left_expr());

  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr());
  if(ctx->ident()) {
      bool isNotArray = false;
      if(ctx->expr() && !Types.isNumericTy(t2)) isNotArray = true;
      t2 = getTypeDecor(ctx->ident());
      if((not Types.isErrorTy(t2))){
          if((not Types.isArrayTy(t2))) {
              Errors.nonArrayInArrayAccess(ctx->ident());
              t2 = Types.createErrorTy();
          }
          else t2 = Types.getArrayElemType(t2);
          if(isNotArray) {
              Errors.nonIntegerIndexInArrayAccess(ctx->expr());
              t2 = Types.createErrorTy();
          }
      }

  }
  if ((not Types.isErrorTy(t1)) and (not getIsLValueDecor(ctx->left_expr())))
    Errors.nonReferenceableLeftExpr(ctx->left_expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isErrorTy(t2)) and (not Types.copyableTypes(t1, t2)))
    Errors.incompatibleAssignment(ctx->ASSIGN());

  DEBUG_EXIT();
}

void TypeCheckListener::enterIfStmt(AslParser::IfStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitIfStmt(AslParser::IfStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1)))
    Errors.booleanRequired(ctx);
  DEBUG_EXIT();
}

void TypeCheckListener::enterWhileStmt(AslParser::WhileStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitWhileStmt(AslParser::WhileStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1)))
    Errors.booleanRequired(ctx);
  DEBUG_EXIT();
}

void TypeCheckListener::enterProcCall(AslParser::ProcCallContext *ctx)
{
        DEBUG_ENTER();
}

void TypeCheckListener::exitProcCall(AslParser::ProcCallContext *ctx)
{
        auto retType = getTypeDecor(ctx->procedure());
        putTypeDecor(ctx, retType);
        DEBUG_EXIT();
}
void TypeCheckListener::enterReadStmt(AslParser::ReadStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitReadStmt(AslParser::ReadStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->left_expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isPrimitiveTy(t1)) and
      (not Types.isFunctionTy(t1)))
    Errors.readWriteRequireBasic(ctx);
  if ((not Types.isErrorTy(t1)) and (not getIsLValueDecor(ctx->left_expr())))
    Errors.nonReferenceableExpression(ctx);
  DEBUG_EXIT();
}

void TypeCheckListener::enterWriteExpr(AslParser::WriteExprContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitWriteExpr(AslParser::WriteExprContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isPrimitiveTy(t1)))
    Errors.readWriteRequireBasic(ctx);
  DEBUG_EXIT();
}

void TypeCheckListener::enterReturnExpr_(AslParser::ReturnExpr_Context *ctx) {
    DEBUG_ENTER();
}

void TypeCheckListener::exitReturnExpr_(AslParser::ReturnExpr_Context *ctx) {
    auto tf = Symbols.getCurrentFunctionTy();
    if (Types.isVoidFunction(tf)) {
        if (ctx->expr()) Errors.incompatibleReturn(ctx->RETURN());
    } else {
        TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
        TypesMgr::TypeId t2 = Types.getFuncReturnType(tf);
        if (not ((Types.isIntegerTy(t1)) and (Types.isFloatTy(t2))) and not Types.equalTypes(t1,t2))
            Errors.incompatibleReturn(ctx->RETURN());

    }
    DEBUG_EXIT();
}

void TypeCheckListener::enterWriteString(AslParser::WriteStringContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitWriteString(AslParser::WriteStringContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterLeft_expr(AslParser::Left_exprContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitLeft_expr(AslParser::Left_exprContext *ctx) {

    TypesMgr::TypeId t = getTypeDecor(ctx->ident());

    if (ctx->INTVAL() or ctx->expr()) {
        if(!Types.isArrayTy(t)){
            if((not Types.isErrorTy(t)))Errors.nonArrayInArrayAccess(ctx->ident());
            t = Types.createErrorTy();

        }
        else t = Types.getArrayElemType(t);
        if (ctx->expr()) {
            TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
            if (!Types.isNumericTy(t1)) {
                t = Types.createErrorTy();
                Errors.nonIntegerIndexInArrayAccess(ctx->expr());
            }
        }

    }
    putTypeDecor(ctx, t);


    bool b = getIsLValueDecor(ctx->ident());
    putIsLValueDecor(ctx, b);
    DEBUG_EXIT();
}

void TypeCheckListener::enterArithmetic(AslParser::ArithmeticContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitArithmetic(AslParser::ArithmeticContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  TypesMgr::TypeId t = Types.createFloatTy();
  if (ctx->expr(1)) {
        TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
        if ((not Types.isErrorTy(t1)) and (not Types.isErrorTy(t2)) and
            ((not Types.isNumericTy(t1)) or (not Types.isNumericTy(t2)))){
                        t = Types.createIntegerTy();
                        Errors.incompatibleOperator(ctx->op);
        }
        else if ((not Types.isErrorTy(t1)) and (not Types.isErrorTy(t2))  and ctx->MOD()
                 and ((not Types.isIntegerTy(t1)) or (not Types.isIntegerTy(t2)))) {
            t = Types.createIntegerTy();
            Errors.incompatibleOperator(ctx->op);
        }
        else if (Types.isIntegerTy(t1) and Types.isIntegerTy(t2)) t = Types.createIntegerTy();
        if(Types.isErrorTy(t1) or Types.isErrorTy(t2)) t = Types.createErrorTy();

  } else {
      if (not Types.isErrorTy(t1) and (not Types.isNumericTy(t1))){
            Errors.incompatibleOperator(ctx->op);
            t = Types.createIntegerTy();
        } else{
            if(Types.isIntegerTy(t1)) t = Types.createIntegerTy();
        }
  }

  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterRelational(AslParser::RelationalContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitRelational(AslParser::RelationalContext *ctx) {
    TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
    TypesMgr::TypeId t = Types.createBooleanTy();
    if (ctx->NOT()) {
        if ((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1))){
            Errors.booleanRequired(ctx);
        }
    } else if (ctx->AND() or ctx->OR()) {
        TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
        std::string oper = ctx->op->getText();
        if (((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1))) or ((not Types.isErrorTy(t2)) and (not Types.isBooleanTy(t2)))){
            Errors.incompatibleOperator(ctx->op);
        }
    } else {
        TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
        std::string oper = ctx->op->getText();
        if ((not Types.isErrorTy(t1)) and (not Types.isErrorTy(t2)) and (not Types.comparableTypes(t1, t2, oper))){
            Errors.incompatibleOperator(ctx->op);
        }

    }
    putTypeDecor(ctx, t);
    putIsLValueDecor(ctx, false);
    DEBUG_EXIT();
}

void TypeCheckListener::enterValue(AslParser::ValueContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitValue(AslParser::ValueContext *ctx) {
    TypesMgr::TypeId t;
    if(ctx->INTVAL()){
        t = Types.createIntegerTy();

    }
    else if(ctx->FLOATVAL()){
        t = Types.createFloatTy();

    }
    else if(ctx->CHARVAL()){
        t = Types.createCharacterTy();

    }
    else if(ctx->BOOLVAL()){
        t = Types.createBooleanTy();

    }
    else{// Type not recognized
        t = Types.createErrorTy();

    }
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterProcedure(AslParser::ProcedureContext *ctx) {
    DEBUG_ENTER();
}

void TypeCheckListener::exitProcedure(AslParser::ProcedureContext *ctx) {
    auto t = getTypeDecor(ctx->ident());
    if (not Types.isFunctionTy(t) and not Types.isErrorTy(t)) {
        Errors.isNotCallable(ctx->ident());
        putTypeDecor(ctx,Types.createErrorTy());
    } else if (Types.isErrorTy(t)) {
        putTypeDecor(ctx,Types.createErrorTy());
    } else if (Types.isFunctionTy(t)) {
        TypesMgr::TypeId t1 = Types.getFuncReturnType(t);
        if (ctx->expr().size() != Types.getNumOfParameters(t)) {

            Errors.numberOfParameters(ctx->ident());
        } else {
            for (unsigned int i = 0; i < ctx->expr().size(); i++) {
                auto expressionType = getTypeDecor(ctx->expr(i));
                auto realType = Types.getParameterType(t,i);
                if (not Types.equalTypes(expressionType, realType) and ((not Types.isFloatTy(realType)) or (not Types.isIntegerTy(expressionType)))) {
                    Errors.incompatibleParameter(ctx->expr(i), i+1, ctx);
                }
            }
        }
        putTypeDecor(ctx,t1);
    }


    DEBUG_EXIT();
}

void TypeCheckListener::enterProcExpr(AslParser::ProcExprContext *ctx) {
    DEBUG_ENTER();
}
void TypeCheckListener::exitProcExpr(AslParser::ProcExprContext *ctx) {
    TypesMgr::TypeId t = getTypeDecor(ctx->procedure());
    if (not Types.isErrorTy(t)) {
        if (Types.isVoidFunction(getTypeDecor(ctx->procedure()->ident()))) {
            Errors.isNotFunction(ctx->procedure()->ident());
            t = Types.createErrorTy();
        }
    }
    putTypeDecor(ctx,t);
    DEBUG_EXIT();
}

void TypeCheckListener::enterExprIdent(AslParser::ExprIdentContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitExprIdent(AslParser::ExprIdentContext *ctx) { 
    TypesMgr::TypeId t = getTypeDecor(ctx->ident());

    if (ctx->INTVAL() or ctx->expr()) {
        if(!Types.isArrayTy(t)){
            t = Types.createErrorTy();
            Errors.nonArrayInArrayAccess(ctx->ident());
        }
        else t = Types.getArrayElemType(t);
        if (ctx->expr()) {
            TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());

            if (!Types.isNumericTy(t1)) {
                t = Types.createErrorTy();
                Errors.nonIntegerIndexInArrayAccess(ctx->expr());
            }
        }


    }
    putTypeDecor(ctx, t);


    bool b = getIsLValueDecor(ctx->ident());
    putIsLValueDecor(ctx, b);
    DEBUG_EXIT();
}


void TypeCheckListener::enterParenthesis(AslParser::ParenthesisContext *ctx) {
    DEBUG_ENTER();
}
void TypeCheckListener::exitParenthesis(AslParser::ParenthesisContext *ctx) {
    TypesMgr::TypeId t = getTypeDecor(ctx->expr());
    putTypeDecor(ctx, t);
    putIsLValueDecor(ctx, false);
    DEBUG_EXIT();
}


void TypeCheckListener::enterIdent(AslParser::IdentContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitIdent(AslParser::IdentContext *ctx) {
  std::string ident = ctx->getText();
  if (Symbols.findInStack(ident) == -1) {
    Errors.undeclaredIdent(ctx->ID());
    TypesMgr::TypeId te = Types.createErrorTy();
    putTypeDecor(ctx, te);
    putIsLValueDecor(ctx, true);
  }
  else {
    TypesMgr::TypeId t1 = Symbols.getType(ident);
    putTypeDecor(ctx, t1);
    if (Symbols.isFunctionClass(ident))
      putIsLValueDecor(ctx, false);
    else
      putIsLValueDecor(ctx, true);
  }
  DEBUG_EXIT();
}

// void TypeCheckListener::enterEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_ENTER();
// }
// void TypeCheckListener::exitEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_EXIT();
// }
// void TypeCheckListener::visitTerminal(antlr4::tree::TerminalNode *node) {
//   DEBUG("visitTerminal");
// }
// void TypeCheckListener::visitErrorNode(antlr4::tree::ErrorNode *node) {
// }


// Getters for the necessary tree node atributes:
//   Scope, Type ans IsLValue
SymTable::ScopeId TypeCheckListener::getScopeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getScope(ctx);
}
TypesMgr::TypeId TypeCheckListener::getTypeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getType(ctx);
}
bool TypeCheckListener::getIsLValueDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getIsLValue(ctx);
}

// Setters for the necessary tree node attributes:
//   Scope, Type ans IsLValue
void TypeCheckListener::putScopeDecor(antlr4::ParserRuleContext *ctx, SymTable::ScopeId s) {
  Decorations.putScope(ctx, s);
}
void TypeCheckListener::putTypeDecor(antlr4::ParserRuleContext *ctx, TypesMgr::TypeId t) {
  Decorations.putType(ctx, t);
}
void TypeCheckListener::putIsLValueDecor(antlr4::ParserRuleContext *ctx, bool b) {
  Decorations.putIsLValue(ctx, b);
}
