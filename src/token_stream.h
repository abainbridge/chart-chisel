#ifndef INCLUDED_TOKEN_STREAM_H
#define INCLUDED_TOKEN_STREAM_H


class TokenStream
{
private:
    char *m_buf;
    char const *m_bufEnd;
    char const *m_pos;  // Points into m_buf;

    char *m_tokenBuf;
    int m_tokenBufSize;

    bool m_resendCurrentToken;

    bool IncPos(int amount);    // Returns false if EOF encountered
    char *CopyToken(char const *startPos, char const *endPos);
    char *GetInteger();   // Only a helper for GetNumber()

    char *GetWord();
    char *GetNumber();
    char *GetString();
    char *GetComment();
//    char *GetArrow();
    char *GetOperator();
    char *GetWhitespace();
    char *GetChar();

public:
    TokenStream();
    bool Open(char const *filename);
    char *GetToken();
    void UnGetToken();
    bool IsEof();
};


#endif
