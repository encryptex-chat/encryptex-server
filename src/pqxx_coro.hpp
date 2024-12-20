#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/asio/append.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/system/detail/error_code.hpp>
#include <pqxx/pqxx>
#include <string>

// NOTE: some tweaking file, dont review
//

template<typename completion_token>
auto execute_db(std::string_view prompt, completion_token&& token)
{
    return boost::asio::async_compose<completion_token, void(std::string)>(
        [prompt](auto&& self) mutable -> std::string
        {
            spdlog::info("{}", prompt);
            return "asdfasdfasdf";
        },
        token);
}

// template<class executor, class handler>
// struct convert
// {
//     auto get_executor() -> executor { return m_wg; }
//
//     convert(std::string prompt, executor e, handler h) :
//         m_prompt(std::move(prompt)), m_wg(e), m_handler(h)
//     {
//     }
//
//     void operator()()
//     {
//         spdlog::info("{}", getenv("ENCRYPTEX_DB"));
//         pqxx::result res;
//         complete(res);
//     }
//
//     private:
//     void complete(boost::system::error_code ec, pqxx::result res)
//     {
//         auto fin = boost::asio::append(std::move(m_handler), res);
//         boost::asio::post(m_wg.get_executor(), std::move(fin));
//     }
//     std::string m_prompt;
//     boost::asio::executor_work_guard<executor> m_wg;
//     handler m_handler;
// };
//
// template<BOOST_ASIO_COMPLETION_TOKEN_FOR(void(pqxx::result)) pqxx_handler>
// auto execute_db(const std::string& prompt, pqxx_handler&& token)
//     -> BOOST_ASIO_INITFN_RESULT_TYPE(pqxx_handler, void(pqxx::result))
// {
//     auto op = [&prompt]<class Handler>(Handler&& handler) mutable
//     {
//         // spdlog::info("{}", getenv("ENCRYPTEX_DB"));
//         // pqxx::connection cx{getenv("ENCRYPTEX_DB")};
//         // spdlog::info("is open {}", cx.is_open());
//         // // pqxx::work tx(cx);
//         // // pqxx::result res = tx.exec(prompt);
//         // // tx.commit();
//         // return pqxx::result{};
//         auto exec = boost::asio::get_associated_executor(handler);
//         auto cv   = convert<decltype(exec), std::decay_t<Handler>>(prompt, exec,
//                                                                    std::forward<Handler>(handler));
//         cv();
//     };
//     return boost::asio::async_initiate<pqxx_handler, void(pqxx::result)>(op, token);
// }

// template<typename Handler = boost::asio::use_awaitable_t<>>
// auto execute_db(const std::string& prompt, Handler&& handler = {})
// {
//     auto init = []<typename Handle>(Handle&& handle, const std::string& msg) mutable
//     {
//         spdlog::info("{}", getenv("ENCRYPTEX_DB"));
//     };
//     return boost::asio::async_initiate<Handler, void(const pqxx::result&)>(init, handler,
//     prompt);
// }
