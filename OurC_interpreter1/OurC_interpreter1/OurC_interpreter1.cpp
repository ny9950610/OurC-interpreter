# include <iostream>
# include <sstream>
# include <cstring>
# include <vector>
# include <stdlib.h>
# include <stdio.h>
# include <iomanip>

#pragma warning(disable : 4996) 

using namespace std;

// token的型別 
enum TokenType {
    IDENT,
    NUM,
    SIGN,
    ADD,       // +
    SUB,       // -
    MUL,       // *
    DIV,       // /
    SEMICOLON, // ;
    ASSIGN,    // :=
    QUIT,      // "quit"
    LBRACKET,  // (
    RBRACKET,  // )
    EQ,        // =
    BT,        // >
    ST,        // <
    BE,        // >=
    SE,        // <=
    NE         // <> 這個是不等於(!=)的意思 
};

// 計算的種類 
enum CalculateType {
    ASSIGN_CAL,  // a:==b ;
    COMPUTE, // a+b ;
    COMPARE  // a>b ;
};

// 數字的種類 
enum NumberType {
    INT,
    FLOAT
};

// 用來儲存token的單位
struct Token {
    TokenType tokenType; // token中的型別 
    char tokenValue[50]; // token中的內容
};

// 用來儲存ident的單位
struct Ident {
    char identName[50];
    float identValue;
    NumberType numberType;
};

// scanner負責把input轉換成token 回傳給parser
class Scanner {
public:
    char mTempTokenValue[50]; // 暫存tokenValue
    TokenType mTempTokenType; // 暫存tokenType
    int mIndex;               // 記錄tokenValue的位置 
    bool mError;              // 紀錄token是否出錯(unrecgonizeTokenWithFirstChar) 
    char mErrorToken;         // 紀錄出錯的token 

    Scanner();

    // 取得一個完整的token
    void GetToken() {
        // 清空tempToken
        ClearToken();
        mIndex = 0;
        mError = false;

        // 以PeekToken來判斷tokenType 再依據tokenType來讀取input
        mTempTokenType = PeekToken();

        // 下一個token是IDENT
        if (mTempTokenType == IDENT) {
            // 將剩下的tokenValue讀完
            while (!IsEndOfIDENT(cin.peek())) {
                mTempTokenValue[mIndex] = GetNextChar();
                mIndex++;
            } // while
            // 如果token為"quit" 那tokenType為QUIT 
            if (strcmp(mTempTokenValue, "quit") == 0) {
                mTempTokenType = QUIT;
            } // if
        } // if
        // 下一個token是NUM
        else if (mTempTokenType == NUM) {
            bool hasDot = false; // 判斷是否有小數點

            // 將剩下的tokenValue讀完
            while (!IsEndOfNUM(cin.peek(), hasDot)) {
                mTempTokenValue[mIndex] = GetNextChar();
                mIndex++;
            } // while
        } // else if
        // 下一個token是ASSIGN 
        else if (mTempTokenType == ASSIGN) {
            mTempTokenValue[mIndex] = GetNextChar();
            // ASSIGN後面必須是'='
            if (cin.peek() == '=') {
                mTempTokenValue[mIndex + 1] = GetNextChar();
            } // if
            // 如果後面不是'=' 那error為':' 
            else {
                mError = true;
                mErrorToken = mTempTokenValue[mIndex];
            } // else
        } // else if
        // 下一個token的第一個char是'>' 
        else if (mTempTokenType == BT) {
            mTempTokenValue[mIndex] = GetNextChar();
            // 檢查是否有下一個char
            if (!IsWhiteSpace(cin.peek())) {
                // 下一個char為'=' token為BE 
                if (cin.peek() == '=') {
                    mTempTokenValue[mIndex + 1] = GetNextChar();
                    mTempTokenType = BE;
                } // if
                // 只有'>'一個char token為BT
            } // if
        } // else if
        // 下一個token的第一個char為'<' 
        else if (mTempTokenType == ST) {
            mTempTokenValue[mIndex] = GetNextChar();
            // 檢查是否有下一個char 
            if (!IsWhiteSpace(cin.peek())) {
                // 下一個char為'=' token為SE
                if (cin.peek() == '=') {
                    mTempTokenValue[mIndex + 1] = GetNextChar();
                    mTempTokenType = SE;
                } // if
                // 下一個char為'>' token為NE
                else if (cin.peek() == '>') {
                    mTempTokenValue[mIndex + 1] = GetNextChar();
                    mTempTokenType = NE;
                } // else if
                // 只有'<'一個char token為ST
            } // if
        } // else if
        // 剩下的token都是只有一個char 直接把char讀進mTempTokenValue中就好 
        else {
            mTempTokenValue[mIndex] = GetNextChar();
        } // else

        // cout << "token: " << mTempTokenValue << endl ;
    } // GetToken()

    // 查看tokenType 但是不讀取 
    TokenType PeekToken() {
        char ch = '\0';
        // 把接下來的token前面的whitespace都讀掉 
        while (IsWhiteSpace(cin.peek())) {
            ch = cin.get();
        } // while 

        // 依照token的第一個char來判斷tokenType
        // "quit"也會進來這個判斷式 要到GetToken才能知道是哪一種tokenType
        if (IsLetter(cin.peek())) {
            return IDENT;
        } // if
        else if (IsDigit(cin.peek()) || cin.peek() == '.') {
            return NUM;
        } // else if
        else if (cin.peek() == '+') {
            return ADD;
        } // else if
        else if (cin.peek() == '-') {
            return SUB;
        } // else if
        else if (cin.peek() == '*') {
            return MUL;
        } // else if
        else if (cin.peek() == '/') {
            char ch = '\0';
            cin.get(ch);
            // 檢查是否為註解 如果是 就把後面的input全部刪除 並重新peekToken 
            if (cin.peek() == '/') {
                ClearInput();
                return PeekToken();
            } // if
            else {
                cin.putback(ch);
                return DIV;
            } // else
        } // else if
        else if (cin.peek() == ';') {
            return SEMICOLON;
        } // else if
        else if (cin.peek() == ':') {
            return ASSIGN;
        } // else if
        else if (cin.peek() == '(') {
            return LBRACKET;
        } // else if
        else if (cin.peek() == ')') {
            return RBRACKET;
        } // else if
        else if (cin.peek() == '=') {
            return EQ;
        } // else if
        // 當token的第一個char為'>' 有可能為 BT( > )或BE( >= )
        else if (cin.peek() == '>') {
            return BT;
        } // else if
        // 當token的第一個char為'>' 有可能為 ST( < )或SE( <= )或NE( <> )
        else if (cin.peek() == '<') {
            return ST;
        } // else if
        // 其他不是token 是error 
        else {
            mError = true;
            // 必須使用cin.peek() 否則跳不出parser的檢查
            mErrorToken = cin.peek();
            // 必須return一個值 所以隨便return一個IDENT
            return IDENT;
        } // else
    } // PeekToken()

    // 把整行的input讀掉
    void ClearInput() {
        char ch = cin.get();
        while (ch != '\n') {
            ch = cin.get();
        } // while
    } // ClearInput()

private:
    // 清除mTempTokenValue的內容
    void ClearToken() {
        int i = 0;
        while (i <= 50) {
            mTempTokenValue[i] = '\0';
            i++;
        } // while
    } // ClearToken()

    // 讀取下一個字元 
    char GetNextChar() {
        char ch = '\0';
        ch = cin.get();

        return ch;
    } // GetNextChar()

    // 若ch為空白or換行 回傳true
    bool IsWhiteSpace(char ch) {
        if (ch == ' ' || ch == '\n' || ch == '\t') {
            return true;
        } // if
        else {
            return false;
        } // else
    } // IsWhiteSpace()

    // 若ch為數字 回傳true
    bool IsDigit(char ch) {
        if (ch >= '0' && ch <= '9') {
            return true;
        } // if
        else {
            return false;
        } // else
    } // IsDigit()

    // 若ch為字母 回傳true
    bool IsLetter(char ch) {
        if (ch >= 'A' && ch <= 'Z') {
            return true;
        } // if
        else if (ch >= 'a' && ch <= 'z') {
            return true;
        } // else if
        else {
            return false;
        } // else
    } // IsLetter()

    // 若ch可為IDENT的結尾 回傳true
    bool IsEndOfIDENT(char ch) {
        if (IsLetter(ch)) {
            return false;
        } // if
        else if (IsDigit(ch)) {
            return false;
        } // else if
        else if (ch == '_') {
            return false;
        } // else if
        else {
            return true;
        } // else
    } // IsEndOfIDENT()

    // 若ch可為NUM的結尾 回傳true 
    bool IsEndOfNUM(char ch, bool& hasDot) {
        if (IsDigit(ch)) {
            return false;
        } // if
        else if (ch == '.' && !hasDot) {
            hasDot = true;
            return false;
        } // else if
        else {
            return true;
        } // else
    } // IsEndOfNUM()

};


Scanner::Scanner() {
    ClearToken();
    mTempTokenType = IDENT;
    mIndex = 0;
    mError = false;
    mErrorToken = '\0';
} // Scanner::Scanner()

// calculator 負責把token串轉換成後序表示法並計算 最後輸出答案
class Calculator {
public:
    vector<Token> mPostfixStack;  // 儲存轉換成後序表式法的token串
    vector<Token> mOperatorStack; // 用來儲存運算子的stack
    vector<float> mAnswerStack;   // 將mPostfixStack中的token拿來做計算
    vector<Ident> mIdents;        // 紀錄Ident
    CalculateType mCalculateType; // 判斷計算的種類
    TokenType mBooleanOperator;   // 當計算為COMPARE時 記錄他的booleanOperator
    NumberType mIntOrFloat;       // 決定要輸出int或是float
    float mAnswer1;               // 用來記錄答案
    float mAnswer2;               // 當計算為COMPARE時 用來記錄第二組答案
    bool mERROR;                  // 其他種類的error ex: 除0
    char mTempIdentName[50];


    void AddNewIdent() {
        Ident temp;
        bool hasSameIdent = false;
        int i = 0;

        // 看這個ident有沒有被宣告過 
        while (i < mIdents.size() && !hasSameIdent) {
            if (strcmp(mTempIdentName, mIdents[i].identName) == 0) hasSameIdent = true;
            i++;
        } // while


        if (hasSameIdent) {
            mIdents[i - 1].identValue = mAnswer1;
            mIdents[i - 1].numberType = mIntOrFloat;
        } // if
        else {
            strcpy(temp.identName, mTempIdentName);
            temp.identValue = mAnswer1;
            temp.numberType = mIntOrFloat;
            mIdents.push_back(temp);
        } // else 
    } // AddNewIdent()

    // 將mToken中的token串轉換成後序表示法 
    void InfixToPostfix(vector<Token> tokens) {
        int i = 0;        // 用來記錄mTokens的位置 
        int j = -1;       // 用來記錄operatorStack的位置 
        char sign = '\0'; // 用來判斷數字是否為正負數 

        // 初始化 
        Initialize();

        while (i < tokens.size()) {
            // 讀進IDENT時 將IDENT記錄在table和mPostfixStack中 quit也算
            if (tokens[i].tokenType == IDENT || tokens[i].tokenType == QUIT) {
                mPostfixStack.push_back(tokens[i]);
            } // if
            // 讀進NUM時 把NUM記錄在mPostfixStack中
            else if (tokens[i].tokenType == NUM) {
                // 根據SIGN把數字加上正負號 
                NumWithSign(sign, tokens[i].tokenValue);
                sign = '\0';

                mPostfixStack.push_back(tokens[i]);
            } // else if
            // 讀進SIGN時 根據tokenValue決定sign
            else if (tokens[i].tokenType == SIGN) {
                sign = tokens[i].tokenValue[0];
            } // else if
            // 讀進加減乘除時 把token放進operatorStack中 
            else if (tokens[i].tokenType == ADD || tokens[i].tokenType == SUB ||
                tokens[i].tokenType == MUL || tokens[i].tokenType == DIV) {
                OperatorPopInPostfix(tokens[i], j);
            } // else if
            // 讀進ASSIGN時 把mCalculateType改成ASSIGN
            else if (tokens[i].tokenType == ASSIGN) {
                mCalculateType = ASSIGN_CAL;
                strcpy(mTempIdentName, mPostfixStack[0].tokenValue);
                mPostfixStack.clear();
            } // else if
            // 讀進LBRACKET時 把hasLBRACKET設為true 方便OperatorPopInPostfix運作
            else if (tokens[i].tokenType == LBRACKET) {
                OperatorPopInPostfix(tokens[i], j);
            } // else if
            // 讀進RBRACKET時 把hasLBRACKET設為false
            else if (tokens[i].tokenType == RBRACKET) {
                OperatorPopInPostfix(tokens[i], j);
            } // else if
            // 剩下的TokenType除了SEMICOLON之外都是屬於COMPARE的token 所以把mCalculateType改成COMPARE 
            else if (tokens[i].tokenType != SEMICOLON) {
                // calculateType改為COMPARE 
                mCalculateType = COMPARE;
                // 設定booleanOperator 
                mBooleanOperator = tokens[i].tokenType;

                // 完成目前的後序式 
                while (!mOperatorStack.empty()) {
                    mPostfixStack.push_back(mOperatorStack.back());
                    mOperatorStack.pop_back();
                } // while

                // 將目前postfixStack中的後序式拿去計算 
                mAnswer1 = CalculateAnswer();
                // 清除postfixStack 
                mPostfixStack.clear();
                mAnswerStack.clear();
                j = -1;
            } // else if

            i++;
        } // while

        // 把operatorStack中的operator全部放進postfixStack中 來完成後序式 
        while (!mOperatorStack.empty()) {
            mPostfixStack.push_back(mOperatorStack.back());
            mOperatorStack.pop_back();
        } // while
    } // InfixToPostfix()

    // 輸出mPostfixStack中的token 
    void PrintPostfix() {
        int i = 0;
        while (i < mPostfixStack.size()) {
            cout << mPostfixStack[i].tokenValue << "  ";
            i++;
        } // while

        cout << endl;
    } // PrintPostfix()

    // 根據calculateType來處理mPostfixStack中的後序計算式
    void StartCalculate() {
        // quit不須做任何計算 直接退出
        if (mPostfixStack[0].tokenType == QUIT) return;

        bool correct = false;
        char answer[50];

        // 根據calculateType來決定接下來的動作 
        if (mCalculateType == COMPUTE) {
            // 把計算結果記在answer1中 
            mAnswer1 = CalculateAnswer();
            CorrectifyOutput(mAnswer1, answer);

            if (mERROR) cout << "Error" << endl;
            // 決定輸出是int或是float
            else if (mIntOrFloat == INT) {
                cout << "> " << answer << endl;
            } // if
            else {
                cout << "> " << answer << endl;
            } // else
        } // if
        else if (mCalculateType == COMPARE) {
            // answer1已經在InfixToPostfix中算完了 接下來只需要計算answer2 
            mAnswer2 = CalculateAnswer();

            // 進行比較並輸出答案 
            correct = Comparsion();
            if (mERROR) cout << "Error" << endl;
            else if (correct) cout << "> true" << endl;
            else cout << "> false" << endl;
        } // else if
        else if (mCalculateType == ASSIGN_CAL) {
            mAnswer1 = CalculateAnswer();
            CorrectifyOutput(mAnswer1, answer);

            if (mERROR) cout << "Error" << endl;
            // 決定輸出是int或是float
            else if (mIntOrFloat == INT) {
                AddNewIdent();
                cout << "> " << answer << endl;
            } // if
            else {
                AddNewIdent();
                cout << "> " << answer << endl;
            } // else
        } // else if
    } // StartCalculate()

private:
    // 初始化變數 
    void Initialize() {
        mPostfixStack.clear();
        mAnswerStack.clear();
        mOperatorStack.clear();
        mCalculateType = COMPUTE;
        mBooleanOperator = IDENT; // 如果不是COMPARE 這個值設為IDENT
        mIntOrFloat = INT;
        mAnswer1 = 0;
        mAnswer2 = 0;
        mERROR = false;
    } // Initialize()

    // 根據SIGN來改變NUM的正負號 
    void NumWithSign(char sign, char tokenValue[50]) {
        int i = strlen(tokenValue);
        int j = 50;

        // 只需要改負號 加號不用改
        if (sign != '-') return;

        // 把tokenValue後面的東西清除 也不知道為什麼會有 清掉就對了
        while (j > i) {
            tokenValue[j] = '\0';
            j--;
        } // while

        // 數字全部往後移一位 
        while (i >= 1) {
            tokenValue[i] = tokenValue[i - 1];
            i--;
        } // while

        // 在tokenValue最前面加上SIGN
        tokenValue[0] = sign;
    } // NumWithSign()

    // 根據優先度把operator放進postfixStack中 
    void OperatorPopInPostfix(Token token, int& j) {
        // 若token為左括號或是operatorStack沒有token 就把目前token放進去
        if (token.tokenType == LBRACKET || j == -1) {
            j++;
            mOperatorStack.push_back(token);
        } // if
        // 若token為右括號 就把左括號之前的token全部放進PostfixStack中 
        else if (token.tokenType == RBRACKET) {
            while (mOperatorStack[j].tokenType != LBRACKET) {
                mPostfixStack.push_back(mOperatorStack[j]);
                mOperatorStack.pop_back();
                j--;
            } // while

            // 把左括號從operatorStack去除 
            mOperatorStack.pop_back();
            j--;
        } // else if
        else {
            // 若token的優先度大於operatorStack中top的優先度 就直接把token放進operatorStack中
            // 否則 把operatorStack中top的值放入postfixStack中 然後拿token和下一個比較
            while (j != -1 && Priority(token) <= Priority(mOperatorStack[j])) {
                mPostfixStack.push_back(mOperatorStack[j]);
                mOperatorStack.pop_back();
                j--;
            } // while

            j++;
            mOperatorStack.push_back(token);
        } // else
    } // OperatorPopInPostfix()

    // 根據operator判斷優先度 
    int Priority(Token token) {
        if (token.tokenType == MUL || token.tokenType == DIV) return 2;
        else if (token.tokenType == ADD || token.tokenType == SUB) return 1;
        else return 0;
    } // Priority()

    // 根據mPostfixStack中的token串得出答案 
    float CalculateAnswer() {
        int i = 0;

        // 從postfixStack的最前面處理 
        while (i < mPostfixStack.size()) {
            // 遇到數字就把token內容放進answerStack 
            if (mPostfixStack[i].tokenType == NUM) {
                mAnswerStack.push_back(ToFloat(mPostfixStack[i].tokenValue));
            } // if
            // 如果遇到operator 就拿answerStack最後面的兩個數字做計算 
            else if (mPostfixStack[i].tokenType == ADD || mPostfixStack[i].tokenType == SUB ||
                mPostfixStack[i].tokenType == MUL || mPostfixStack[i].tokenType == DIV) {
                mAnswerStack.push_back(Calculate(mPostfixStack[i].tokenType));
            } // else if
            // 如果遇到ident 就去idents找此ident的值放進answerStack
            else if (mPostfixStack[i].tokenType == IDENT) {
                mAnswerStack.push_back(GetIdentValue(mPostfixStack[i].tokenValue));
            } // else if

            i++;
        } // while

        // 算到最後 answerStack[0]為答案 
        return mAnswerStack[0];
    } // CalculateAnswer()

    // 根據op,num1,num2來計算
    float Calculate(TokenType op) {
        float num1 = 0;
        float num2 = 0;
        float answer = 0;

        // 取出answerStack最後面的兩個數字 
        num2 = mAnswerStack.back();
        mAnswerStack.pop_back();
        num1 = mAnswerStack.back();
        mAnswerStack.pop_back();

        // 根據operator來計算 
        if (op == ADD) {
            answer = num1 + num2;
        } // if
        else if (op == SUB) {
            answer = num1 - num2;
        } // else if
        else if (op == MUL) {
            answer = num1 * num2;
        } // else if
        else if (op == DIV) {
            // 不能夠除0 記錄此error 
            if (num2 == 0) mERROR = true;
            else answer = num1 / num2;
        } // else if

        return answer;
    } // Calculate()


    float GetIdentValue(char tokenValue[50]) {
        int i = 0;
        while (i < mIdents.size()) {
            if (strcmp(tokenValue, mIdents[i].identName) == 0) {
                if (mIdents[i].numberType == FLOAT) mIntOrFloat = FLOAT;
                return mIdents[i].identValue;
            } // if

            i++;
        } // while

        return 0;
    } // GetIdentValue()

    // 比較answer1和answer2 並根據是否正確來輸出答案 
    bool Comparsion() {
        // 若兩個數字的誤差在0.0001以內就不計
        if (mAnswer1 - mAnswer2 < 0.0001 && mAnswer2 - mAnswer1 < 0.0001) mAnswer1 = mAnswer2;
        if (mBooleanOperator == EQ && mAnswer1 == mAnswer2) return true;
        else if (mBooleanOperator == NE && mAnswer1 != mAnswer2) return true;
        else if (mBooleanOperator == BT && mAnswer1 > mAnswer2) return true;
        else if (mBooleanOperator == BE && mAnswer1 >= mAnswer2) return true;
        else if (mBooleanOperator == ST && mAnswer1 < mAnswer2) return true;
        else if (mBooleanOperator == SE && mAnswer1 <= mAnswer2) return true;
        else return false;
    } // Comparsion()

    // 讓數字能夠符合輸出 注意:bug很多 要仔細處理 
    void CorrectifyOutput(float num, char str[50]) {
        bool hasDot = false;
        int counter = 0;
        int i = 0;

        // 把數字轉成char字串 
        sprintf(str, "%f", num);

        // 輸出為int型別 
        if (mIntOrFloat == INT) {
            while (i < 50) {
                if (str[i] == '.') hasDot = true;
                if (hasDot) str[i] = '\0';

                i++;
            } // while
        } // if
        // 輸出為float型別 
        else if (mIntOrFloat == FLOAT) {
            while (i < 50) {
                // 小數不需四捨五入的狀況 
                if (counter == 3 && str[i] < '5') str[i] = '\0';
                // 小數需要四捨五入的狀況 
                else if (counter == 3 && str[i] >= '5') {
                    num = num + 0.001;
                    sprintf(str, "%f", num);

                    i = 0;
                    hasDot = false;
                    counter = 0;
                    while (i < 50) {
                        if (counter >= 3) str[i] = '\0';
                        if (hasDot) counter++;
                        if (str[i] == '.') hasDot = true;

                        i++;
                    } // while
                } // else if

                if (hasDot) counter++;
                if (str[i] == '.') hasDot = true;

                i++;
            } // while
        } // else if


    } // CorrectifyOutput()

    // 把型別為char字串的num轉成float
    float ToFloat(char num[50]) {
        int i = 0;
        int j = 0;
        int divideNum = 1;    // 用來記錄小數後有幾位 
        bool negative = false;
        bool hasDot = false;
        float answer = 0;

        while ((num[i] == '-' || (num[i] >= '0' && num[i] <= '9') || num[i] == '.')) {
            if (num[i] == '-') {
                negative = true;
            } // if
            else if (num[i] == '.') {
                mIntOrFloat = FLOAT;
                hasDot = true;
            } // else if
            else {
                answer = answer * 10 + num[i] - '0';
                if (hasDot) {
                    // 只要hasDot為true 每讀進一個數字就紀錄這個小數加一位 
                    divideNum = divideNum * 10;
                    j++;
                } // if
            } // else if

            i++;
        } // while

        answer = answer / divideNum;

        // 處理負數的問題 
        if (negative) answer = 0 - answer;
        return answer;
    } // ToFloat()

}; // Calculator()

// parser負責檢查文法 並呼叫calculator來計算
class Parser {
public:
    Scanner mScanner;
    Calculator mCalculator;
    vector<Token> mTokens; // 用linklist來儲存Token
    bool mSyntaxError;     // 用來判斷token串的文法是否正確
    bool mSemanticError;
    bool mError;
    char mSyntaxErrorToken[50];
    char mSemanticErrorToken[50];

    Parser();

    // 在tokens新增一個token
    void AddNewToken() {
        Token temp;
        strcpy(temp.tokenValue, mScanner.mTempTokenValue);
        temp.tokenType = mScanner.mTempTokenType;

        mTokens.push_back(temp);
    } // AddNewToken()

    // 把整行的input讀掉
    void ClearInput() {
        mScanner.ClearInput();
    } // ClearInput()

    // 輸出tokens
    void PrintTokens() {
        int i = 0;
        while (i < mTokens.size()) {
            cout << mTokens[i].tokenValue << "  ";
            i++;
        } // while

        cout << endl;
    } // PrintTokens()

    // 檢查文法 若沒有錯誤 回傳true
    bool IsRightGrammer() {
        if (Is_Command()) return true;
        else return false;
    } // IsRightGrammer()

    // 根據tokens執行計算 
    void Calculation() {
        mCalculator.InfixToPostfix(mTokens);
        mCalculator.StartCalculate();
    } // Calculation()

    // 根據讀取的tokens來判斷要如何輸出
    void DecideOutput() {
        if (mScanner.mError) {
            cout << "> Unrecognized token with first char : '" << mScanner.mErrorToken << "'" << endl;
            mScanner.mError = false;
            mScanner.mErrorToken = '\0';
            ClearInput();
        } // if
        else if (mError) {
            mError = false;
            cout << "Error" << endl;
        } // else if 
        // 如果文法有錯誤 在mTokens中新增一個節點 避免迴圈判斷式因為沒有結點可以讀取而出bug
        else if (mSyntaxError && strcmp(mSyntaxErrorToken, "\0") != 0) {
            cout << "> Unexpected token : '" << mSyntaxErrorToken << "'" << endl;

            strcpy(mSyntaxErrorToken, "\0");
            ClearInput();

            Token temp;
            temp.tokenType = IDENT;
            strcpy(temp.tokenValue, "\0");
            mTokens.push_back(temp);
        } // else if
        else if (mSemanticError && strcmp(mSemanticErrorToken, "\0") != 0) {
            mSemanticError = false;
            cout << "> Undefined identifier : '" << mSemanticErrorToken << "'" << endl;

            strcpy(mSemanticErrorToken, "\0");
            ClearInput();
        } // else if
        else {
            Calculation();
        } // else
    } // DecideOutput()
private:
    // 檢查接下來的token是否符合<Command>的文法
    bool Is_Command() {
        // <Command> ::= IDENT ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ' ;'
        // 提醒: IDENT會有undefine token的error 到時候判斷式要再加上是否define的條件 
        if (mScanner.PeekToken() == IDENT) {
            mScanner.GetToken();
            AddNewToken();

            // <Command> ::= QUIT 因為在PeekToken時quit會被當成IDENT 要到GetToken才會知道正確的tokenType
            if (mTokens[0].tokenType == QUIT) {
                return true;
            } // if

            // 檢查是否為undefined token
            if (NoSameIdent()) {
                // 如果下個token不符合文法 為syntax error 
                if (mScanner.PeekToken() == LBRACKET || mScanner.PeekToken() == RBRACKET ||
                    mScanner.PeekToken() == NUM || mScanner.PeekToken() == IDENT) {
                    mScanner.GetToken();
                    strcpy(mSyntaxErrorToken, mScanner.mTempTokenValue);
                    return false;
                } // if
                // 如果下一個token為ASSIGN就不需檢查
                else if (mScanner.PeekToken() != ASSIGN) {
                    strcpy(mSemanticErrorToken, mScanner.mTempTokenValue);
                    mSemanticError = true;
                    return false;
                } // if
                // 剩下的情況都是undefined token 
            } // if

            // <Command> ::= IDENT ':=' <ArithExp> ' ;'
            if (mScanner.PeekToken() == ASSIGN) {
                mScanner.GetToken();
                AddNewToken();

                if (Is_ArithExp(true)) {
                    if (mScanner.PeekToken() == SEMICOLON) {
                        mScanner.GetToken();
                        AddNewToken();

                        return true;
                    } // if
                } // if
            } // if
            // <Command> ::= IDENT <IDlessArithExpOrBexp> ' ;'
            else if (Is_IDlessArithExpOrBexp()) {
                if (mScanner.PeekToken() == SEMICOLON) {
                    mScanner.GetToken();
                    AddNewToken();

                    return true;
                } // if
            } // else if
        } // if
        // <Command> ::= <NOT_IDStartArithExpOrBexp> ' ;'
        else if (Is_NOT_ID_StartArithExpOrBexp()) {
            if (mScanner.PeekToken() == SEMICOLON) {
                mScanner.GetToken();
                AddNewToken();

                return true;
            } // if 
        } // else if

        if (!mSemanticError) {
            mScanner.GetToken();
            strcpy(mSyntaxErrorToken, mScanner.mTempTokenValue);
        } // if

        return false;
    } // Is_Command()

    // 檢查接下來的token是否符合<Is_NOT_ID_StartArithExpOrBexp>的文法 
    bool Is_NOT_ID_StartArithExpOrBexp() {
        bool grammerIsRight = false;
        if (Is_ArithExp(false)) {
            grammerIsRight = true;

            if (Is_BooleanOperator()) {
                grammerIsRight = Is_ArithExp(true);
            } // if
        } // if

        return grammerIsRight;
    } // Is_NOT_ID_StartArithExpOrBexp()

    // 檢查接下來的token是否符合<IDlessArithExpOrBexp>的文法
    bool Is_IDlessArithExpOrBexp() {
        bool grammerIsRight = true;
        // 檢查<IDlessArithExpOrBexp>前面的迴圈部分
        while (mScanner.PeekToken() == ADD || mScanner.PeekToken() == SUB ||
            mScanner.PeekToken() == MUL || mScanner.PeekToken() == DIV) {
            mScanner.GetToken();
            AddNewToken();

            // 檢查<IDlessArithExpOrBexp>前面的迴圈部分( {   '+' <Term>   | '-' <Term> } )
            if (mScanner.mTempTokenType == ADD || mScanner.mTempTokenType == SUB) {
                grammerIsRight = Is_Term(true);
            } // if
            // 檢查<IDlessArithExpOrBexp>前面的迴圈部分( { '*' <Factor> | '/' <Factor> } )
            else if (mScanner.mTempTokenType == MUL || mScanner.mTempTokenType == DIV) {
                grammerIsRight = Is_Factor(true);
            } // else if

            if (!grammerIsRight) return false;
        } // while

        // <IDlessArithExpOrBexp>後面的部分可有可沒有( [ <BooleanOperator> <ArithExp> ] ) 
        if (Is_BooleanOperator()) {
            grammerIsRight = Is_ArithExp(true);
        } // if

        return grammerIsRight;
    } // Is_IDlessArithExpOrBexp()

    // 檢查接下來的token是否符合<BooleanOperator>的文法
    bool Is_BooleanOperator() {
        // <BooleanOprator> ::= '=' | '<>' | '>' | '<' | '>=' | '<='
        if (mScanner.PeekToken() == EQ || mScanner.PeekToken() == NE || mScanner.PeekToken() == BT ||
            mScanner.PeekToken() == ST || mScanner.PeekToken() == BE || mScanner.PeekToken() == SE) {
            mScanner.GetToken();
            AddNewToken();

            return true;
        } // if

        return false;
    } // Is_BooleanOperator()

    // 檢查接下來的token是否符合<ArithExp>的文法
    bool Is_ArithExp(bool IDStart) {
        bool grammerIsRight = true;
        // <ArithExp> ::= <Term>

        if (!Is_Term(IDStart)) grammerIsRight = false;

        // 檢查<Term>後面的迴圈部分( { '+' <Term> | '-' <Term> } )
        while (grammerIsRight && (mScanner.PeekToken() == ADD || mScanner.PeekToken() == SUB)) {
            mScanner.GetToken();
            AddNewToken();

            grammerIsRight = Is_Term(true);
        } // while

        return grammerIsRight;
    } // Is_ArithExp()

    // 檢查接下來的token是否符合<Term>的文法
    bool Is_Term(bool IDStart) {
        bool grammerIsRight = true;

        if (!Is_Factor(IDStart)) grammerIsRight = false;

        // 檢查<Term>後面的迴圈部分( { '+' <Term> | '-' <Term> } )
        while (grammerIsRight && (mScanner.PeekToken() == MUL || mScanner.PeekToken() == DIV)) {
            mScanner.GetToken();
            AddNewToken();

            grammerIsRight = Is_Factor(true);
        } // while

        return grammerIsRight;
    } // Is_Term()

    // 檢查接下來的token是否符合<Factor>的文法
    bool Is_Factor(bool IDStart) {
        // <Factor> ::= IDENT
        if (mScanner.PeekToken() == IDENT && IDStart) {
            mScanner.GetToken();
            AddNewToken();

            // 檢查是否undefined token
            if (NoSameIdent()) {
                strcpy(mSemanticErrorToken, mScanner.mTempTokenValue);
                mSemanticError = true;
                return false;
            } // if

            return true;
        } // if
        // <Factor> ::= [ SIGN ] NUM
        else if (mScanner.PeekToken() == ADD || mScanner.PeekToken() == SUB) {
            mScanner.GetToken();
            // 把tokenValue改成SIGN 之後才能把數字改成正負數
            mScanner.mTempTokenType = SIGN;
            AddNewToken();

            if (mScanner.PeekToken() == NUM) {
                mScanner.GetToken();
                AddNewToken();

                return true;
            } // if
        } // else if
        // <Factor> ::= NUM
        else if (mScanner.PeekToken() == NUM) {
            mScanner.GetToken();
            AddNewToken();

            return true;
        } // else if
        // <Factor> ::= '( ' <ArithExp> ' )'
        else if (mScanner.PeekToken() == LBRACKET) {
            mScanner.GetToken();
            AddNewToken();

            if (Is_ArithExp(true)) {
                if (mScanner.PeekToken() == RBRACKET) {
                    mScanner.GetToken();
                    AddNewToken();

                    return true;
                } // if 
            } // if
        } // else if

        return false;
    } // Is_Factor()

    // 當tokens最新讀進來的token為ident時 檢查他有沒有被宣告過
    bool NoSameIdent() {
        int i = 0;
        while (i < mCalculator.mIdents.size()) {
            if (strcmp(mTokens.back().tokenValue, mCalculator.mIdents[i].identName) == 0) return false;
            i++;
        } // while

        return true;
    } // NoSameIdent()

}; // Parser()


Parser::Parser() {
    mTokens.clear();
    mSyntaxError = false;
    mSemanticError = false;
    mError = false;
    strcpy(mSyntaxErrorToken, "\0");
    strcpy(mSemanticErrorToken, "\0");
} // Parser::Parser()


int main() {
    int uTestNum = 0;
    // cin >> uTestNum ; // 注意 在外面測試時一定要把這行註解掉!!!!!!!!

    Parser parser;

    if (false) return 0; // 這句是用來卡styleCheck的

    cout << "Program starts..." << endl;

    do {
        // 把上一次的token串全部刪除
        parser.mTokens.clear();
        // 依序讀取token並判斷文法
        parser.mSyntaxError = !parser.IsRightGrammer();
        // 根據讀取的token串來判斷要如何輸出 
        parser.DecideOutput();
    } while (parser.mTokens[0].tokenType != QUIT);

    cout << "> Program exits...";

} // main()
