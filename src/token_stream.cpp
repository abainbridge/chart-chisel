// Own header
#include "token_stream.h"

// Standard headers
#include <ctype.h>
#include <stdio.h>
#include <string.h>


TokenStream::TokenStream()
{
    m_buf = NULL;
    m_bufEnd = NULL;
    m_tokenBufSize = 1024;
    m_tokenBuf = new char [m_tokenBufSize];
    m_resendCurrentToken = false;
    m_currentLineNum = 1;
}


bool TokenStream::Open(char const *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        return false;
    }

    fseek(f, 0, SEEK_END);
    int bufSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    m_buf = new char [bufSize + 1];
    fread(m_buf, 1, bufSize, f);
    fclose(f);

    m_buf[bufSize] = '\0';
    m_pos = m_buf;
    m_bufEnd = m_buf + bufSize;

    return true;
}


bool TokenStream::IncPos(int amount)
{
    for (int i = 0; i < amount; i++) {
        m_pos++;
        if (*m_pos == '\n')
            m_currentLineNum++;

        if (m_pos >= m_bufEnd) {
            m_pos = m_bufEnd;
            return false;
        }
    }

    return true;
}


// endPos is actually one beyond the end of the token
char *TokenStream::CopyToken(char const *startPos, char const *endPos)
{
    int len = endPos - startPos;
    if (len <= 0) {
        return NULL;
    }
    memcpy(m_tokenBuf, startPos, len);
    m_tokenBuf[len] = '\0';
    return m_tokenBuf;
}


char *TokenStream::GetWord()
{
    char const *startPos = m_pos;

    while (isalnum(*m_pos) || *m_pos == '_')
        if (!IncPos(1))
            break;
    
    return CopyToken(startPos, m_pos);
}


char *TokenStream::GetInteger()
{
    char const *startPos = m_pos;

    while (isdigit(*m_pos))
        if (!IncPos(1))
            break;

    return CopyToken(startPos, m_pos);
}


char *TokenStream::GetNumber()
{
    // Accepts numbers like: <integer>
    //                       <.><decimal>
    //                       <integer><.><decimal>

    char const *startPos = m_pos;

    if (*m_pos == '.')
    {
        // Do annoying look ahead to see if this is a number at all
        if (m_pos < m_bufEnd && isdigit(*(m_pos+1)))
        {
            IncPos(1);
            GetInteger();
        }
    }
    else
    {
        if (!GetInteger())
            return NULL;
        if (*m_pos == '.')
        {
            IncPos(1);
            GetInteger();
        }
    }

    return CopyToken(startPos, m_pos);
}


char *TokenStream::GetString()
{
    char const *startPos = m_pos;

    if (*m_pos == '\'')
    {
        IncPos(1);
        while (*m_pos != '\'')
            if (!IncPos(1))
                break;
        if (*m_pos == '\'')
            IncPos(1);
    }
    else if (*m_pos == '"')
    {
        IncPos(1);
        while (*m_pos != '"')
            if (!IncPos(1))
                break;
        if (*m_pos == '"')
            IncPos(1);
    }

    return CopyToken(startPos, m_pos);
}


char *TokenStream::GetComment()
{
    char const *startPos = m_pos;

    if (*m_pos == '/')
    {
        // Do annoying look ahead to see if this is a comment
        if (m_pos < m_bufEnd && *(m_pos+1) == '*')
            IncPos(2);
        else
            return NULL;

        while (*(m_pos - 1) != '*' || *m_pos != '/')
            if (!IncPos(1))
                break;
    }

    return CopyToken(startPos, m_pos);
}


// char const *TokenStream::GetArrow()
// {
//     char const *startPos = m_pos;
// 
//     if (*m_pos == '-' && m_pos < m_bufEnd && *(m_pos+1) == '>')
//     {
//         IncPos(2);
//         return CopyToken(startPos, m_pos);
//     }
//     
//     return NULL;
// }


char *TokenStream::GetOperator()
{
    char const *startPos = m_pos;
    char const operatorChars[] = "|<>&!%^*-+=/.,";
    
    if (!strchr(operatorChars, *m_pos))
        return NULL;

    do
    {
        IncPos(1);
    } while (strchr(operatorChars, *m_pos));

    return CopyToken(startPos, m_pos);
}


char *TokenStream::GetWhitespace()
{
    char const *startPos = m_pos;

    while (isspace(*m_pos))
        if (!IncPos(1))
            break;

    return CopyToken(startPos, m_pos);
}


char *TokenStream::GetChar()
{
    char const *startPos = m_pos;
    IncPos(1);
    return CopyToken(startPos, m_pos);
}


char *TokenStream::GetToken()
{
    if (m_resendCurrentToken)
    {
        m_resendCurrentToken = false;
    }
    else
    {
        while (GetComment() || GetWhitespace()) ;
//        GetWord() || GetNumber() || GetString() || GetArrow() || GetChar();
        GetWord() || GetNumber() || GetString() || GetOperator() || GetChar();
    }
    
    return m_tokenBuf;
}


void TokenStream::UnGetToken()
{
    m_resendCurrentToken = true;
}


bool TokenStream::IsEof()
{
    if (m_resendCurrentToken)
        return false;
    return m_pos >= m_bufEnd;
}
