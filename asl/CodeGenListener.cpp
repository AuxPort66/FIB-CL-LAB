//////////////////////////////////////////////////////////////////////
//
//    CodeGenListener - Walk the parser tree to do
//                             the generation of code
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

#include "CodeGenListener.h"

#include "antlr4-runtime.h"

#include "../common/TypesMgr.h"
#include "../common/SymTable.h"
#include "../common/TreeDecoration.h"
#include "../common/code.h"

#include <cstddef>    // std::size_t

// uncomment the following line to enable debugging messages with DEBUG*
// #define DEBUG_BUILD
#include "../common/debug.h"

// using namespace std;


// Constructor
CodeGenListener::CodeGenListener(TypesMgr       & Types,
                                 SymTable       & Symbols,
                                 TreeDecoration & Decorations,
                                 code           & Code) :
  Types{Types},
  Symbols{Symbols},
  Decorations{Decorations},
  Code{Code} {
}

void CodeGenListener::enterProgram(AslParser::ProgramContext *ctx) {
  DEBUG_ENTER();
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
}
void CodeGenListener::exitProgram(AslParser::ProgramContext *ctx) {
  Symbols.popScope();
  DEBUG_EXIT();
}

void CodeGenListener::enterFunction(AslParser::FunctionContext *ctx) {
  DEBUG_ENTER();
  subroutine subr(ctx->ID()->getText());
  Code.add_subroutine(subr);
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
  codeCounters.reset();
  subroutine & subrRef = Code.get_last_subroutine();
  
  Symbols.setCurrentFunctionTy(getTypeDecor(ctx));
  if (not Types.isVoidFunction(Symbols.getCurrentFunctionTy())) {
                  subrRef.add_param("_result");
  }
  
}
void CodeGenListener::exitFunction(AslParser::FunctionContext *ctx) {
  subroutine & subrRef = Code.get_last_subroutine();
  instructionList code = getCodeDecor(ctx->statements());
  if (not Types.isVoidFunction(Symbols.getCurrentFunctionTy())) {
    code = getCodeDecor(ctx->statements());
    if ((code.size()-1) >= 0) {
        if (code[code.size()-1].oper != instruction::RETURN().oper) code = code || instruction::RETURN();
    }
  }
  if (code.size() == 0) code = code || instruction::RETURN();
  else if (code.size() > 0) {
      if (code[code.size()-1].oper != instruction::RETURN().oper) code = code || instruction::RETURN();
  }
  subrRef.set_instructions(code);
  Symbols.popScope();
  DEBUG_EXIT();
}

void CodeGenListener::enterDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_EXIT();
}

void CodeGenListener::enterParameter_decl(AslParser::Parameter_declContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitParameter_decl(AslParser::Parameter_declContext *ctx) {
  subroutine       & subrRef = Code.get_last_subroutine();
  TypesMgr::TypeId        t1;
  if(ctx->type())  t1 = getTypeDecor(ctx->type());
  else t1 = getTypeDecor(ctx->array_decl());
  
  for(auto id : ctx->ID()){
                  subrRef.add_param(id->getText());
  }
  DEBUG_EXIT();
}

void CodeGenListener::enterVariable_decl(AslParser::Variable_declContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitVariable_decl(AslParser::Variable_declContext *ctx) {
  subroutine       & subrRef = Code.get_last_subroutine();
  TypesMgr::TypeId        t1;
  if(ctx->type())  t1 = getTypeDecor(ctx->type());
  else t1 = getTypeDecor(ctx->array_decl());
  std::size_t           size = Types.getSizeOfType(t1);
  for(auto id : ctx->ID()){
                  subrRef.add_var(id->getText(), size);
  }
  DEBUG_EXIT();
}


void CodeGenListener::enterType(AslParser::TypeContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitType(AslParser::TypeContext *ctx) {
  DEBUG_EXIT();
}

void CodeGenListener::enterStatements(AslParser::StatementsContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitStatements(AslParser::StatementsContext *ctx) {
  instructionList code;
  for (auto stCtx : ctx->statement()) {
    code = code || getCodeDecor(stCtx);
  }
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterAssignStmt(AslParser::AssignStmtContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitAssignStmt(AslParser::AssignStmtContext *ctx) {
            auto addr1 = getAddrDecor(ctx->left_expr());
            auto offs1 = getOffsetDecor(ctx->left_expr());
            auto code1 = getCodeDecor(ctx->left_expr());
            TypesMgr::TypeId t1 = getTypeDecor(ctx->left_expr());
            auto addr2 = getAddrDecor(ctx->expr());
            std::string     offs2 = getOffsetDecor(ctx->expr());
            auto code2 = getCodeDecor(ctx->expr());
            TypesMgr::TypeId t2 = getTypeDecor(ctx->expr());
            auto code = code1 || code2;
            if (ctx->left_expr()->expr() or ctx->left_expr()->INTVAL()) {
                t1 = getTypeDecor(ctx->ident());
                
                if (Symbols.isParameterClass(addr1)) {
                    std::string temp3 = "%"+codeCounters.newTEMP();
                    code = code || instruction::LOAD(temp3,getAddrDecor(ctx->ident()));
                    addr1 = temp3;
                }
                
                if (ctx->ident()) {
                    
                    t2 = getTypeDecor(ctx->ident());
                    t2 = Types.getArrayElemType(t2);
                    
                    auto temp3 = "%" + codeCounters.newTEMP();
                    code = code || instruction::LOAD(temp3,offs2);
                    offs2 = temp3;
                    
                    auto temp = "%" + codeCounters.newTEMP();
                    addr2 = getAddrDecor(ctx->ident());
                    
                    if (arrayLocs.count(addr2)) addr2 = arrayLocs[addr2];
                    
                    code = code || instruction::LOADX(temp, addr2,offs2);
                    addr2 = temp;
                }

                if(Types.isIntegerTy(t2) and Types.isFloatTy(t1)) {
                    auto tmp = "%" + codeCounters.newTEMP();
                    code = code  || instruction::FLOAT(tmp,addr2);
                    addr2 = tmp;
                }
                code = code || instruction::XLOAD(addr1, offs1, addr2);

            } else if (ctx->ident()) {
                t2 = getTypeDecor(ctx->ident());
                t2 = Types.getArrayElemType(t2);
                addr2 = getAddrDecor(ctx->ident());

                if(Types.isIntegerTy(t2) and Types.isFloatTy(t1)) {
                    auto temp = "%" + codeCounters.newTEMP();
                    code = code || instruction::LOADX(temp, addr2,offs2);
                    auto tmp = "%" + codeCounters.newTEMP();
                    code = code  || instruction::FLOAT(tmp,temp);
                    code = code || instruction::LOAD(addr1, temp);
                }
                else code = code || instruction::LOADX(addr1, addr2,offs2);

            } else if(Types.isArrayTy(t2)){
                if (Symbols.isParameterClass(addr1)) {
                    auto temp = "%" + codeCounters.newTEMP();
                    code = code || instruction::ALOAD(temp, addr2) || instruction::LOAD(addr1, temp);
                } else {
                    auto temp = "%" + codeCounters.newTEMP();
                    code = code || instruction::LOAD(temp, addr2);
                    arrayLocs[addr1] = temp;
                }
            } else {
                if(Types.isIntegerTy(t2) and Types.isFloatTy(t1)) {
                    auto tmp = "%" + codeCounters.newTEMP();
                    code = code  || instruction::FLOAT(tmp,addr2);
                    addr2 = tmp;
                }
                code = code || instruction::LOAD(addr1, addr2);
            }

            putCodeDecor(ctx, code);
            DEBUG_EXIT();
         


}

void CodeGenListener::enterIfStmt(AslParser::IfStmtContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitIfStmt(AslParser::IfStmtContext *ctx) {
  instructionList   code;
  std::string      addr1 = getAddrDecor(ctx->expr());
  instructionList  code1 = getCodeDecor(ctx->expr());
  instructionList  code2 = getCodeDecor(ctx->statements(0));
  std::string      label = codeCounters.newLabelIF();
  std::string labelEndIf = "endif"+label;
  if(ctx->ELSE()){
      std::string labelElse = "else"+label;
      instructionList code3 = getCodeDecor(ctx->statements(1));
      code = code1 || instruction::FJUMP(addr1, labelElse) ||
         code2 || instruction::UJUMP(labelEndIf) ||
         instruction::LABEL(labelElse) || code3 || instruction::LABEL(labelEndIf);
  }
  else{
      code = code1 || instruction::FJUMP(addr1, labelEndIf) ||
         code2 || instruction::LABEL(labelEndIf);
  }
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterWhileStmt(AslParser::WhileStmtContext *ctx){
    DEBUG_ENTER();
}
void CodeGenListener::exitWhileStmt(AslParser::WhileStmtContext *ctx){
    instructionList   code;
    std::string      addr1 = getAddrDecor(ctx->expr());
    instructionList  code1 = getCodeDecor(ctx->expr());
    instructionList  code2 = getCodeDecor(ctx->statements());
    std::string      label = codeCounters.newLabelWHILE();
    std::string labelStartWhile = "startWhile"+label;
    std::string labelWhile = "while"+label;
    std::string labelEndwhile = "endwhile"+label;
    code = instruction::LABEL(labelStartWhile) || code1 || instruction::LABEL(labelWhile)||
            instruction::FJUMP(addr1, labelEndwhile) ||code2 ||
            instruction::UJUMP(labelStartWhile) ||instruction::LABEL(labelEndwhile);

    putCodeDecor(ctx, code);
    DEBUG_EXIT();
}


void CodeGenListener::enterProcCall(AslParser::ProcCallContext *ctx) {
        DEBUG_ENTER();
}
void CodeGenListener::exitProcCall(AslParser::ProcCallContext *ctx) {
    auto functionType = getTypeDecor(ctx->procedure()->ident());
    instructionList code = getCodeDecor(ctx->procedure());
    if (not Types.isVoidFunction(functionType)) code = instruction::PUSH() || code || instruction::POP();
    putCodeDecor(ctx, code);
        
    DEBUG_EXIT();
}

void CodeGenListener::enterReadStmt(AslParser::ReadStmtContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitReadStmt(AslParser::ReadStmtContext *ctx) {
  instructionList  code;
  std::string     addr1 = getAddrDecor(ctx->left_expr());
  std::string     offs1 = getOffsetDecor(ctx->left_expr());
  instructionList code1 = getCodeDecor(ctx->left_expr());
  TypesMgr::TypeId tid1 = getTypeDecor(ctx->left_expr());
  if (ctx->left_expr()->INTVAL() or ctx->left_expr()->expr()) {
        std::string temp = "%"+codeCounters.newTEMP();
        if(Types.isFloatTy(tid1)) code = code1 || instruction::READF(temp);
        else if(Types.isCharacterTy(tid1)) code = code1 || instruction::READC(temp);
        else code = code1 || instruction::READI(temp);
        code = code || instruction::XLOAD(addr1,offs1,temp);
  } else {
    if(Types.isFloatTy(tid1)) code = code1 || instruction::READF(addr1);
    else if(Types.isCharacterTy(tid1)) code = code1 || instruction::READC(addr1);
    else code = code1 || instruction::READI(addr1);
  }
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterWriteExpr(AslParser::WriteExprContext *ctx) {
  DEBUG_ENTER();
}

void CodeGenListener::exitWriteExpr(AslParser::WriteExprContext *ctx) {
  instructionList code;
  std::string     addr1 = getAddrDecor(ctx->expr());
  instructionList code1 = getCodeDecor(ctx->expr());
  TypesMgr::TypeId tid1 = getTypeDecor(ctx->expr());
  if(Types.isFloatTy(tid1)) code = code1 || instruction::WRITEF(addr1);
  else if(Types.isCharacterTy(tid1)) code = code1 || instruction::WRITEC(addr1);
  else code = code1 || instruction::WRITEI(addr1);
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterReturnExpr_(AslParser::ReturnExpr_Context *ctx) {
    DEBUG_ENTER();
}

void CodeGenListener::exitReturnExpr_(AslParser::ReturnExpr_Context *ctx) {
    instructionList code;
    if (ctx->expr()) {
        std::string     addr1 = getAddrDecor(ctx->expr());
        instructionList code1 = getCodeDecor(ctx->expr());
        
        TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
        TypesMgr::TypeId t2 = Types.getFuncReturnType(Symbols.getCurrentFunctionTy()); 
        if ((Types.isIntegerTy(t1)) and (Types.isFloatTy(t2))) {
            std::string temp = "%"+codeCounters.newTEMP();
            code = code1 || instruction::FLOAT(temp, addr1) || instruction::LOAD("_result",addr1);            
        } else {
            code = code1 || instruction::LOAD("_result",addr1);
        }
    }
    code = code || instruction::RETURN();
    putCodeDecor(ctx, code);
    DEBUG_EXIT();
}

void CodeGenListener::enterWriteString(AslParser::WriteStringContext *ctx) {
    DEBUG_ENTER();
}
void CodeGenListener::exitWriteString(AslParser::WriteStringContext *ctx) {
  instructionList code;
  std::string s = ctx->STRING()->getText();
  std::string temp = "%"+codeCounters.newTEMP();
  int i = 1;
  while (i < int(s.size())-1) {
    if (s[i] != '\\') {
      code = code ||
             instruction::CHLOAD(temp, s.substr(i,1)) ||
             instruction::WRITEC(temp);
      i += 1;
    }
    else {
      assert(i < int(s.size())-2);
      if (s[i+1] == 'n') {
        code = code || instruction::WRITELN();
        i += 2;
      }
      else if (s[i+1] == 't' or s[i+1] == '"' or s[i+1] == '\\') {
        code = code ||
               instruction::CHLOAD(temp, s.substr(i,2)) ||
               instruction::WRITEC(temp);
        i += 2;
      }
      else {
        code = code ||
               instruction::CHLOAD(temp, s.substr(i,1)) ||
               instruction::WRITEC(temp);
        i += 1;
      }
    }
  }
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterLeft_expr(AslParser::Left_exprContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitLeft_expr(AslParser::Left_exprContext *ctx) {
    auto addr1 = getAddrDecor(ctx->ident());
    auto code = getCodeDecor(ctx->ident());
  
    if (arrayLocs.count(addr1)) addr1 = arrayLocs[addr1];
  if (ctx->INTVAL()) {
      std::string temp = "%"+codeCounters.newTEMP();
      
      code = code || instruction::LOAD(temp,ctx->INTVAL()->getText());
      
      if (Symbols.isParameterClass(addr1)) {
          std::string temp3 = "%"+codeCounters.newTEMP();
          code = code || instruction::LOAD(temp3,getAddrDecor(ctx->ident()));
          addr1 = temp3;
      }
      
      putOffsetDecor(ctx, temp);
      putCodeDecor(ctx, code);
  }
  else if (ctx->expr()) {
      
      if (Symbols.isParameterClass(addr1)) {
          std::string temp3 = "%"+codeCounters.newTEMP();
          code = code || instruction::LOAD(temp3,getAddrDecor(ctx->ident()));
          addr1 = temp3;
      }
      
      putOffsetDecor(ctx, getOffsetDecor(ctx->expr()));
      putCodeDecor(ctx, code);
  }
  else {
      putOffsetDecor(ctx, getOffsetDecor(ctx->ident()));
      putCodeDecor(ctx, code);
  }
  putAddrDecor(ctx, addr1);
  DEBUG_ENTER();
}

void CodeGenListener::enterArithmetic(AslParser::ArithmeticContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitArithmetic(AslParser::ArithmeticContext *ctx) {
    std::string     addr1 = getAddrDecor(ctx->expr(0));
    instructionList code1 = getCodeDecor(ctx->expr(0));
    TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
    std::string temp = "%"+codeCounters.newTEMP();
    instructionList code;
    
    if (not ctx->expr(1)) {
        code = code1;
        if (ctx->RES()) {
            if (Types.isIntegerTy(t1)) code = code || instruction::NEG(temp,addr1);
            else code = code || instruction::FNEG(temp,addr1);
        }
    } else {
        
        std::string     addr2 = getAddrDecor(ctx->expr(1));
        instructionList code2 = getCodeDecor(ctx->expr(1));
        TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
        code  = code1 || code2;
    
        if(Types.isFloatTy(t1) or Types.isFloatTy(t2)){
            if(Types.isIntegerTy(t1)) {
                std::string tmp = "%"+codeCounters.newTEMP();
                code = code || instruction::FLOAT(tmp,addr1);
                addr1 = tmp;
            }
            else if(Types.isIntegerTy(t2)){
                std::string tmp = "%"+codeCounters.newTEMP();
                code = code || instruction::FLOAT(tmp,addr2);
                addr2 = tmp;
            }
            if (ctx->MUL())
                code = code || instruction::FMUL(temp, addr1, addr2);
            else  if (ctx->DIV())
                code = code || instruction::FDIV(temp, addr1, addr2);
            else  if (ctx->RES())
                code = code || instruction::FSUB(temp, addr1, addr2);
            else // (ctx->PLUS())
                code = code || instruction::FADD(temp, addr1, addr2);
        }
        else{
            if (ctx->MOD()) {
                std::string temp3 = "%"+codeCounters.newTEMP();
                code = code || instruction::DIV(temp3, addr1, addr2) || instruction::MUL(temp3, temp3, addr2) || instruction::SUB(temp, addr1, temp3);
            }
            else if (ctx->MUL())
                code = code || instruction::MUL(temp, addr1, addr2);
            else  if (ctx->DIV())
                code = code || instruction::DIV(temp, addr1, addr2);
            else  if (ctx->RES())
                code = code || instruction::SUB(temp, addr1, addr2);
            else // (ctx->PLUS())
                code = code || instruction::ADD(temp, addr1, addr2);
        }
    }
    putAddrDecor(ctx, temp);
    putOffsetDecor(ctx, temp);
    putCodeDecor(ctx, code);
    DEBUG_EXIT();
}

void CodeGenListener::enterParenthesis(AslParser::ParenthesisContext *ctx) {
    DEBUG_ENTER();
}
void CodeGenListener::exitParenthesis(AslParser::ParenthesisContext *ctx) {
    std::string     addr1 = getAddrDecor(ctx->expr());
    instructionList code1 = getCodeDecor(ctx->expr());
    putAddrDecor(ctx, addr1);
    putOffsetDecor(ctx, addr1);
    putCodeDecor(ctx, code1);
    DEBUG_EXIT();
}

void CodeGenListener::enterRelational(AslParser::RelationalContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitRelational(AslParser::RelationalContext *ctx) {
    std::string temp = "%"+codeCounters.newTEMP();
    std::string     addr1 = getAddrDecor(ctx->expr(0));
    instructionList code1 = getCodeDecor(ctx->expr(0));
    instructionList code;
    if (ctx->NOT()) {
        code = code1 || instruction::NOT(temp, addr1);
    } else {
        std::string     addr2 = getAddrDecor(ctx->expr(1));
        instructionList code2 = getCodeDecor(ctx->expr(1));
        code  = code1 || code2;
        TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
        TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
        if(Types.isFloatTy(t1) or Types.isFloatTy(t2)){
            if(Types.isIntegerTy(t1)) {
                std::string tmp = "%"+codeCounters.newTEMP();
                code = code || instruction::FLOAT(tmp,addr1);
                addr1 = tmp;
            }
            else if(Types.isIntegerTy(t2)){
                std::string tmp = "%"+codeCounters.newTEMP();
                code = code || instruction::FLOAT(tmp,addr2);
                addr2 = tmp;
            }
            if (ctx->EQUAL()) code = code || instruction::FEQ(temp, addr1, addr2);
            else if(ctx->DIF()) {
                code  = code || instruction::FEQ(temp,addr1,addr2);
                std::string temp2= "%"+codeCounters.newTEMP();
                code = code || instruction::NOT(temp2,temp);
            }
            else if(ctx->BT()) code = code || instruction::FLE(temp,addr2,addr1);
            else if(ctx->BE()) code = code || instruction::FLT(temp,addr2,addr1);
            else if(ctx->LT()) code = code || instruction::FLT(temp,addr1,addr2);
            else if(ctx->LE()) code = code || instruction::FLE(temp,addr1,addr2);
        }
        else{
            if (ctx->EQUAL()) code = code || instruction::EQ(temp, addr1, addr2);
            else if(ctx->DIF()) {
                code  = code || instruction::EQ(temp,addr1,addr2);
                std::string temp2= "%"+codeCounters.newTEMP();
                code = code || instruction::NOT(temp2,temp);
            }
            else if(ctx->BT()) code = code || instruction::LE(temp,addr2,addr1);
            else if(ctx->BE()) code = code || instruction::LT(temp,addr2,addr1);
            else if(ctx->LT()) code = code || instruction::LT(temp,addr1,addr2);
            else if(ctx->LE()) code = code || instruction::LE(temp,addr1,addr2);
            else if (ctx->AND()) code = code || instruction::AND(temp, addr1, addr2);
            else if (ctx->OR())  code = code || instruction::OR(temp, addr1, addr2);
        }
    } 
    putAddrDecor(ctx, temp);
    putOffsetDecor(ctx, temp);
    putCodeDecor(ctx, code);
    DEBUG_EXIT();
}

void CodeGenListener::enterValue(AslParser::ValueContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitValue(AslParser::ValueContext *ctx) {
  instructionList code;
  std::string temp = "%"+codeCounters.newTEMP();
  std::string text = ctx->getText();
  if (ctx->INTVAL()){
    code = instruction::ILOAD(temp, text);
    putOffsetDecor(ctx, text);
  }
  if (ctx->FLOATVAL()){
    code = instruction::FLOAD(temp, text);

  }
  if (ctx->CHARVAL()){
      if (text.size() == 3) code = instruction::CHLOAD(temp, text.substr(1,1));
    else code = instruction::CHLOAD(temp, text.substr(1,2));

  }
  if(ctx->BOOLVAL()){
    if(text == "true"){
      text = '1';
    }
    else {
      text = '0';

    }
    code = instruction::ILOAD(temp, text);
  }
  putAddrDecor(ctx, temp);
  
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterProcedure(AslParser::ProcedureContext *ctx) {
    DEBUG_ENTER();
}

void CodeGenListener::exitProcedure(AslParser::ProcedureContext *ctx) {
    
    //I do the initial push to proccall and procexpr
    auto functionNode = ctx->ident();
    auto functionType = getTypeDecor(functionNode);
    instructionList code;
    int currentParameter = 0;
    //Sum code of all expressions
    //Declare parametres
    for (auto params : ctx->expr()) {
        std::string     addrTemp = getAddrDecor(params);
        instructionList codeTemp = getCodeDecor(params);
        auto t = getTypeDecor(params);
        
        if (Types.isArrayTy(t)) {
            if (arrayLocs.count(getAddrDecor(params))) {
                addrTemp = arrayLocs[getAddrDecor(params)];
            } else {
                std::string temp = "%"+codeCounters.newTEMP();
                codeTemp = codeTemp || instruction::ALOAD(temp, addrTemp);
                addrTemp = temp;
                arrayLocs[getAddrDecor(params)] = temp;
            }
            
        } 
        //see if you need to cast an int on a float
        auto t2 = Types.getParameterType(functionType,currentParameter);
        if (Types.isIntegerTy(t) and Types.isFloatTy(t2)) {
            std::string temp = "%"+codeCounters.newTEMP();
            codeTemp = codeTemp || instruction::FLOAT(temp, addrTemp);
            addrTemp = temp;
        }
        code = code || codeTemp || instruction::PUSH(addrTemp);
        currentParameter++;
    }
    code = code || instruction::CALL(functionNode->ID()->getText());
    //Pop Parameters
    for (int i = ctx->expr().size()-1; i >= 0; i--) {
        auto params = ctx->expr(i);
        auto addr1 = getAddrDecor(params);
        auto funcType = getTypeDecor(ctx->ident());
        auto parType = Types.getParameterType(funcType,i);
        if (Types.isArrayTy(parType)) {
            if (not arrayLocs.count(addr1)) {
                std::string temp = "%"+codeCounters.newTEMP();
                arrayLocs[addr1] = temp;
                code = code || instruction::POP(temp);
            }
            else code = code || instruction::POP(arrayLocs[addr1]);
        } else code = code || instruction::POP();
    }
    
    putCodeDecor(ctx, code);
    
    DEBUG_EXIT();
}

void CodeGenListener::enterProcExpr(AslParser::ProcExprContext *ctx) {
    DEBUG_ENTER();
}

void CodeGenListener::exitProcExpr(AslParser::ProcExprContext *ctx) {
    instructionList code1 = getCodeDecor(ctx->procedure());
    
    std::string temp = "%"+codeCounters.newTEMP();
    
    code1 = instruction::PUSH() || code1 || instruction::POP(temp);
    
    putCodeDecor(ctx,code1); 
    putAddrDecor(ctx, temp); 
    putOffsetDecor(ctx, temp);
    
    DEBUG_EXIT();
}

void CodeGenListener::enterExprIdent(AslParser::ExprIdentContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitExprIdent(AslParser::ExprIdentContext *ctx) {
  auto identType = getTypeDecor(ctx->ident());
  auto addr1 = getAddrDecor(ctx->ident());
    
  if (ctx->INTVAL()) {
      
        std::string temp = "%"+codeCounters.newTEMP();
        std::string temp2 = "%"+codeCounters.newTEMP();
        instructionList code  = instruction::LOAD(temp2,ctx->INTVAL()->getText());
        
        if (Symbols.isParameterClass(addr1) and not arrayLocs.count(addr1)) {
            std::string temp3 = "%"+codeCounters.newTEMP();
            code = code || instruction::LOAD(temp3,getAddrDecor(ctx->ident()));
            addr1 = temp3;
        }
        code = code || instruction::LOADX(temp,addr1,temp2);
        putOffsetDecor(ctx, temp);
        putCodeDecor(ctx, code);
        putAddrDecor(ctx, temp);
      
      
  }
  else if (ctx->expr()){
      instructionList code = getCodeDecor(ctx->expr());
      
      std::string temp = "%"+codeCounters.newTEMP();
        auto identType = getTypeDecor(ctx->ident());
        if (Types.isArrayTy(identType)) {
            if (not arrayLocs.count(addr1)) {
                std::string temp2 = "%"+codeCounters.newTEMP();
                code = code || instruction::LOAD(temp2,getAddrDecor(ctx->ident()));
                addr1 = temp2;
            } else addr1 = arrayLocs[addr1];
        }
      
      code = code || instruction::LOADX(temp,addr1,getOffsetDecor(ctx->expr()));
      putOffsetDecor(ctx, temp);
      putCodeDecor(ctx, code);
      putAddrDecor(ctx, temp);
  }
  else {
      putAddrDecor(ctx, addr1);
      putOffsetDecor(ctx, getOffsetDecor(ctx->ident()));
      putCodeDecor(ctx, getCodeDecor(ctx->ident()));
  }
  DEBUG_EXIT();
}

void CodeGenListener::enterIdent(AslParser::IdentContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitIdent(AslParser::IdentContext *ctx) {

  putAddrDecor(ctx, ctx->ID()->getText());
  putOffsetDecor(ctx, ctx->ID()->getText());
  putCodeDecor(ctx, instructionList());
  DEBUG_EXIT();
}

// void CodeGenListener::enterEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_ENTER();
// }
// void CodeGenListener::exitEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_EXIT();
// }
// void CodeGenListener::visitTerminal(antlr4::tree::TerminalNode *node) {
//   DEBUG(">>> visit " << node->getSymbol()->getLine() << ":" << node->getSymbol()->getCharPositionInLine() << " CodeGen TerminalNode");
// }
// void CodeGenListener::visitErrorNode(antlr4::tree::ErrorNode *node) {
// }


// Getters for the necessary tree node atributes:
//   Scope, Type, Addr, Offset and Code
SymTable::ScopeId CodeGenListener::getScopeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getScope(ctx);
}
TypesMgr::TypeId CodeGenListener::getTypeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getType(ctx);
}
std::string CodeGenListener::getAddrDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getAddr(ctx);
}
std::string  CodeGenListener::getOffsetDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getOffset(ctx);
}
instructionList CodeGenListener::getCodeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getCode(ctx);
}

// Setters for the necessary tree node attributes:
//   Addr, Offset and Code
void CodeGenListener::putAddrDecor(antlr4::ParserRuleContext *ctx, const std::string & a) {
  Decorations.putAddr(ctx, a);
}
void CodeGenListener::putOffsetDecor(antlr4::ParserRuleContext *ctx, const std::string & o) {
  Decorations.putOffset(ctx, o);
}
void CodeGenListener::putCodeDecor(antlr4::ParserRuleContext *ctx, const instructionList & c) {
  Decorations.putCode(ctx, c);
}
