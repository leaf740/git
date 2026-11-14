#pragma once
#include <cstdint>
#include <utility>
#include <variant>
namespace fw{enum class Error:std::uint8_t{Ok=0,NotInitialized,InvalidArgument,Timeout,Busy,Io,Checksum,Unsupported,QueueFull,Internal};template<class T>class Result{public:static Result success(T x){return Result(std::move(x));}static Result failure(Error e){return Result(e);}bool ok()const noexcept{return std::holds_alternative<T>(v_);}explicit operator bool()const noexcept{return ok();}Error error()const noexcept{return ok()?Error::Ok:std::get<Error>(v_);}T&value(){return std::get<T>(v_);}private:explicit Result(T x):v_(std::move(x)){}explicit Result(Error e):v_(e){}std::variant<T,Error>v_;};template<>class Result<void>{public:static Result success(){return Result(Error::Ok);}static Result failure(Error e){return Result(e);}bool ok()const noexcept{return e_==Error::Ok;}explicit operator bool()const noexcept{return ok();}Error error()const noexcept{return e_;}private:explicit Result(Error e):e_(e){}Error e_;};}
