#ifndef ast_Statement_hpp
#define ast_Statement_hpp

#include "ast_ASTNode.hpp"

class ReturnStatement : public ASTNode
{
    public:
        node expression;

        ReturnStatement(node _expression):
            expression(_expression){}
        ReturnStatement():
            expression(){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"return";
            if(expression!=NULL){
                dst<<" ";
                expression->print(dst, state);
            }
            dst<<";";
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"return";
            if(expression!=NULL){
                dst<<" ";
                expression->translate(dst, state);
            }
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            if(expression!=NULL){
                int reg1 = state.getTempReg(0,dst);
                expression->compile(dst,state);
                state.ifLoad(dst,reg1);
                dst<<"addu"<<" "<<"$"<<"2"<<" , "<<"$"<<reg1<<" , "<<"$"<<"0"<<std::endl;
                state.registers[reg1]=0;
                state.ifFull(dst);
            }
            dst<<"b"<<" "<<"$E"<<state.returnId<<std::endl;
            dst<<"nop"<<std::endl;
        }

        //for frame size
        void count(CompilerState &state) const override {
            if(expression!=NULL){
                expression->count(state);
            }
        }
};

class ExprStatement : public ASTNode
{
    public:
        node expression;

        ExprStatement(node _expression):
            expression(_expression){}
        ExprStatement():
            expression(NULL){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            if(expression!=NULL){
                for(int i=state.indent;i!=0;i--){
                    dst<<"\t";
                }
                expression->print(dst, state);
                dst<<";";
            }
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            if(expression!=NULL){
                for(int i=state.indent;i!=0;i--){
                    dst<<"\t";
                }
                expression->translate(dst, state);
            }
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            if(expression!=NULL){
                int reg1 = state.getTempReg(0,dst);
                expression->compile(dst,state);
                state.ifLoad(dst,reg1);
                state.registers[reg1]=0;
                state.ifFull(dst);
            }
        }

        //for frame size
        void count(CompilerState &state) const override {
            if(expression!=NULL){
                expression->count(state);
            }
        }
};

class DeclareStatement : public ASTNode
{
    public:
        std::string type;
        node decList;

        DeclareStatement(std::string &_type, node _decList):
        type(_type), decList(_decList){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<type<<" ";
            state.currentType = type;

            decList->print(dst,state);

            dst<<";";
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            decList->translate(dst,state);
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            state.currentType = type;
            decList->compile(dst,state);
        }

        //for frame size
        void count(CompilerState &state) const override {
            state.currentType = type;
            decList->count(state);
        }
};

class Declare : public ASTNode
{
    public:
        std::string id;
        node expression;

        Declare(std::string &_id, node _expression):
        id(_id), expression(_expression){}
        Declare(std::string &_id):
        id(_id), expression(NULL){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            dst<<id;
            if(expression!=NULL){
                dst<<"=";
                expression->print(dst, state);
            }
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            dst<<id;
            if(expression!=NULL){
                dst<<"=";
                expression->translate(dst, state);
            } else {
                dst<<"=0";
            }
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            int offset=state.offset(state.currentType);
            state.varVector.push_back(VariableBind(id, state.currentType, state.currentScope,offset));
            if(expression!=NULL){
                int reg1 = state.getTempReg(0,dst);
                expression->compile(dst,state);
                state.ifLoad(dst,reg1);
                if(((!state.currentType.compare("char"))||(!state.currentType.compare("signed char")))||
                    (!state.currentType.compare("unsigned char"))){
                    dst<<"sb "<<"$"<<reg1<<" , "<<offset<<"($fp)"<<std::endl;
                } else if((!state.currentType.compare("short"))||(!state.currentType.compare("unsigned short"))){
                    dst<<"sh "<<"$"<<reg1<<" , "<<offset<<"($fp)"<<std::endl;
                } else{
                    dst<<"sw "<<"$"<<reg1<<" , "<<offset<<"($fp)"<<std::endl;
                }
                state.registers[reg1]=0;
                state.ifFull(dst);
            } else {
                int reg1 = state.getTempReg(1,dst);
                dst<<"addi"<<" "<<"$"<<reg1<<" , "<<"$"<<"0"<<" , "<<"0x0"<<std::endl;
                if(((!state.currentType.compare("char"))||(!state.currentType.compare("signed char")))||
                    (!state.currentType.compare("unsigned char"))){
                    dst<<"sb "<<"$"<<reg1<<" , "<<offset<<"($fp)"<<std::endl;
                } else if((!state.currentType.compare("short"))||(!state.currentType.compare("unsigned short"))){
                    dst<<"sh "<<"$"<<reg1<<" , "<<offset<<"($fp)"<<std::endl;
                } else{
                    dst<<"sw "<<"$"<<reg1<<" , "<<offset<<"($fp)"<<std::endl;
                }
                state.registers[reg1]=0;
                state.ifFull(dst);
            }
        }

        //for frame size
        void count(CompilerState &state) const override {
            if(expression!=NULL){
                expression->count(state);
            }

            std::string temp=state.currentType;

            state.varCount=state.varCount+state.typeToSize(state.currentType);
        }
};

class GlobalDeclare : public ASTNode
{
    public:
        std::string id;
        double input;
        bool noInput;

        GlobalDeclare(std::string &_id, double _input):
        id(_id), input(_input), noInput(false){}
        GlobalDeclare(std::string &_id):
        id(_id), noInput(true){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            dst<<id;
            if(noInput==false){
                dst<<"="<<input;
            }
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            state.gloVariables.push_back(id);
            dst<<id;
            if(noInput==false){
                dst<<"="<<input;
            } else {
                dst<<"=0";
            }
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            state.gloVarVector.push_back(GloVarBind(id,state.currentType));
            dst<<".globl "<<id<<std::endl;
	        dst<<".data "<<std::endl;
	        dst<<".align 2"<<std::endl;
            dst<<id<<":"<<std::endl;
            if(noInput==false){
                dst<<".word "<<input<<std::endl;
            } else {
                dst<<".word "<<"0"<<std::endl;
            }
            dst<<".text"<<std::endl;
        }

        virtual void count(CompilerState &state) const override {}
};


class NewScope : public ASTNode
{
    public:
        node body;

        NewScope(node _body): 
            body(_body){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"{"<<std::endl;
            state.indent++;
            body->print(dst, state);
            state.indent--;
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"}";
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            body->translate(dst, state);
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            state.currentScope++;
            body->compile(dst, state);
            state.popScope();
            state.currentScope--;
        }

        //for frame size
        void count(CompilerState &state) const override {
            body->count(state);
        }
};

class If_Statement : public ASTNode
{
    public:
        node condition;
        node statement1;
    
        If_Statement(node _condition, node _statement1):
        condition(_condition), statement1(_statement1){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"if (";
            condition->print(dst,state);
            dst<<")"<<std::endl;
            state.indent++;
            statement1->print(dst,state);
            state.indent--;
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"if (";
            condition->translate(dst,state);
            dst<<") :"<<std::endl;
            state.indent++;
            statement1->translate(dst,state);
            state.indent--;
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            int reg1 = state.getTempReg(0,dst);
            condition->compile(dst,state);
            state.ifLoad(dst,reg1);
            state.registers[reg1]=0;
            state.ifFull(dst);
            
            dst<<"beq"<<" "<<"$"<<reg1<<" , "<<"$"<<"0"<<" , "<<"$L"<<(state.labelId)<<std::endl;
            dst<<"nop"<<std::endl;

            statement1->compile(dst,state);

            dst<<"$L"<<state.label()<<":"<<std::endl;
        }

        //for frame size
        void count(CompilerState &state) const override {
            condition->count(state);
            statement1->count(state);
        }
};

class IfElse_Statement : public ASTNode
{
    public:
        node condition;
        node statement1;
        node statement2;
    
        IfElse_Statement(node _condition, node _statement1, node _statement2):
        condition(_condition), statement1(_statement1), statement2(_statement2){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"if (";
            condition->print(dst,state);
            dst<<")"<<std::endl;
            state.indent++;
            statement1->print(dst,state);
            dst<<std::endl;
            state.indent--;
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"else";
            dst<<std::endl;
            state.indent++;
            statement2->print(dst,state);
            dst<<std::endl;
            state.indent--;
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"if (";
            condition->translate(dst,state);
            dst<<") :"<<std::endl;
            state.indent++;
            statement1->translate(dst,state);
            dst<<std::endl;
            state.indent--;
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"else :";
            dst<<std::endl;
            state.indent++;
            statement2->translate(dst,state);
            dst<<std::endl;
            state.indent--;
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            int reg1 = state.getTempReg(0,dst);
            condition->compile(dst,state);
            state.ifLoad(dst,reg1);
            state.registers[reg1]=0;
            state.ifFull(dst);
            
            dst<<"beq"<<" "<<"$"<<reg1<<" , "<<"$"<<"0"<<" , "<<"$L"<<(state.labelId)<<std::endl;
            dst<<"nop"<<std::endl;

            statement1->compile(dst,state);

            dst<<"b"<<" "<<"$L"<<state.labelId+1<<std::endl;
            dst<<"nop"<<std::endl;

            dst<<"$L"<<state.label()<<":"<<std::endl;
            statement2->compile(dst,state);
            dst<<"$L"<<state.label()<<":"<<std::endl;
        }

        //for frame size
        void count(CompilerState &state) const override {
            condition->count(state);
            statement1->count(state);
            statement2->count(state);
        }
};

class While_Statement : public ASTNode
{
    public:
        node condition;
        node statement;
    
        While_Statement(node _condition, node _statement):
        condition(_condition), statement(_statement){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"While (";
            condition->print(dst, state);
            dst<<")"<<std::endl;
            state.indent++;
            statement->print(dst, state);
            state.indent--;
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"while (";
            condition->translate(dst, state);
            dst<<") :"<<std::endl;
            state.indent++;
            statement->translate(dst, state);
            state.indent--;
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            int loopLabel = state.label();
            int exitLabel = state.label();
            dst<<"$L"<<loopLabel<<":"<<std::endl;

            int reg1 = state.getTempReg(0,dst);
            condition->compile(dst,state);
            state.ifLoad(dst,reg1);
            state.registers[reg1]=0;
            state.ifFull(dst);
            
            dst<<"beq"<<" "<<"$"<<reg1<<" , "<<"$"<<"0"<<" , "<<"$L"<<(exitLabel)<<std::endl;
            dst<<"nop"<<std::endl;

            statement->compile(dst,state);

            dst<<"b"<<" "<<"$L"<<(loopLabel)<<std::endl;
            dst<<"nop"<<std::endl;
            dst<<"$L"<<exitLabel<<":"<<std::endl;
        }

        //for frame size
        void count(CompilerState &state) const override {
            condition->count(state);
            statement->count(state);
        }
};

class Do_While_Statement : public ASTNode
{
    public:
        node body;
        node condition;
    
        Do_While_Statement(node _body, node _condition):
        body(_body), condition(_condition){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"do"<<std::endl;
            state.indent++;
            body->print(dst, state);
            state.indent--;
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"while ";
            condition->print(dst, state);
            dst<<";";
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            /*for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"While ";
            condition->translate(dst, state);
            dst<<" :"<<std::endl;
            state.indent++;
            body->translate(dst, state);
            state.indent--;*/
            //not sure how to do this
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            
            int loopLabel = state.label();
            int exitLabel = state.label();
            dst<<"$L"<<loopLabel<<":"<<std::endl;

            body->compile(dst,state);

            int reg1 = state.getTempReg(0,dst);
            condition->compile(dst,state);
            state.ifLoad(dst,reg1);
            state.registers[reg1]=0;
            state.ifFull(dst);
            
            dst<<"beq"<<" "<<"$"<<reg1<<" , "<<"$"<<"0"<<" , "<<"$L"<<(exitLabel)<<std::endl;
            dst<<"nop"<<std::endl;

            dst<<"b"<<" "<<"$L"<<(loopLabel)<<std::endl;
            dst<<"nop"<<std::endl;
            dst<<"$L"<<exitLabel<<":"<<std::endl;
        }

        //for frame size
        void count(CompilerState &state) const override {
            condition->count(state);
            body->count(state);
        }
};

class ForStatement : public ASTNode
{
    public:
        node statement1;
        node condition;
        node statement2;
        node body;
    
        ForStatement(node _statement1, node _condition, node _statement2, node _body):
            statement1(_statement1), condition(_condition), statement2(_statement2), body(_body){}
        ForStatement(node _statement1, node _condition, node _body):
            statement1(_statement1), condition(_condition), statement2(NULL), body(_body){}

        //print tester
        virtual void print(std::ostream &dst, PrintTransState &state) const override
        {
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"For (";
            statement1->print(dst, state);
            condition->print(dst, state);
            if(statement2!=NULL){ statement2->print(dst, state); }
            dst<<")"<<std::endl;

            state.indent++;
            body->print(dst, state);
            state.indent--;
        }

        //translator 
        virtual void translate(std::ostream &dst, PrintTransState &state) const override{
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            statement1->translate(dst, state);
            dst<<std::endl;
            for(int i=state.indent;i!=0;i--){
                dst<<"\t";
            }
            dst<<"While ";
            condition->translate(dst, state);
            dst<<" :"<<std::endl;
            
            state.indent++;
            body->translate(dst, state);
            state.indent--;
            if(statement2!=NULL){ statement2->translate(dst,state); }
            dst<<std::endl;
        }

        //compiler 
        virtual void compile(std::ostream &dst, CompilerState &state) const override{
            state.currentScope++;
            
            statement1->compile(dst,state);

            int loopLabel = state.label();
            int exitLabel = state.label();
            dst<<"$L"<<loopLabel<<":"<<std::endl;

            int reg1 = state.getTempReg(0,dst);

            condition->compile(dst,state);
            state.ifLoad(dst,reg1);
            state.registers[reg1]=0;
            state.ifFull(dst);

            dst<<"beq"<<" "<<"$"<<reg1<<" , "<<"$"<<"0"<<" , "<<"$L"<<(exitLabel)<<std::endl;
            dst<<"nop"<<std::endl;

            body->compile(dst,state);
            if(statement2!=NULL){ statement2->compile(dst,state); }

            state.popScope();
            state.currentScope--;

            dst<<"b"<<" "<<"$L"<<(loopLabel)<<std::endl;
            dst<<"nop"<<std::endl;
            dst<<"$L"<<exitLabel<<":"<<std::endl;
        }

        //for frame size
        void count(CompilerState &state) const override {
            statement1->count(state);
            condition->count(state);
            body->count(state);
            statement2->count(state);
        }
};

#endif
