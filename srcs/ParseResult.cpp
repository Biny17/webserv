#include "../headers/ParseResult.hpp"
#include "../headers/utils.hpp"

void ParseResult::Error(std::string msg, int error_code)
{
    state = ERROR_STATE;
    ok = false;
    if (msg == "")
        err.msg = "malformed request syntax";
    else
        err.msg = msg;
    err.error_code = error_code;
}

void ParseResult::Method(const std::string& buff, size_t& i)
{
    parse_token(buff, req.method, i);
    if (i == buff.length())
        return;
    if (buff[i] == ' ') {
        state = PATH;
        i++;
    }
    else
        Error("", 400);
}

void ParseResult::Path(const std::string& buff, size_t& i)
{

}

void ParseResult::FillReq(const std::string& buff)
{

    size_t i = 0;
    if (this->state == p_state::INIT)
        this->state = METHOD;
    if (this->state == p_state::METHOD)
        Method(buff, i);
    if (this->state == p_state::PATH)
        Path(buff, i);
}