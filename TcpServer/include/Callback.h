#ifndef CALLBACK_H
#define CALLBACK_H

#include <cstring>

template <typename F>
class Callback;

template<typename R>
class Callback<R()> {
public:
    Callback(R (*func)() = 0) {
        if (!func) {
            memset(this, 0, sizeof(Callback));
        } else {
            generate(func);
        }
    }

    template<typename T, typename U>
    Callback(U *obj, R (T::*method)()) {
        generate(method_context<T, R (T::*)()>(obj, method));
    }

    R call() const {
        return _ops->call(this);
    }

    R operator()() const {
        return call();
    }

private:
    // Stored as pointer to function and pointer to optional object
    // Function pointer is stored as union of possible function types
    // to guarantee proper size and alignment
    struct _class;
    union {
        void (*_staticfunc)();
        void (*_boundfunc)(_class*);
        void (_class::*_methodfunc)();
    } _func;
    void *_obj;

    // Dynamically dispatched operations
    const struct ops {
        R (*call)(const void*);
        void (*move)(void*, const void*);
        void (*dtor)(void*);
    } *_ops;

    // Generate operations for function object
    template <typename F>
    void generate(const F &f) {
        static const ops ops = {
            &Callback::function_call<F>,
            &Callback::function_move<F>,
            &Callback::function_dtor<F>,
        };

        //MBED_STATIC_ASSERT(sizeof(Callback) - sizeof(_ops) >= sizeof(F),
        //        "Type F must not exceed the size of the Callback class");
        memset(this, 0, sizeof(Callback));
        new (this) F(f);
        _ops = &ops;
    }

    // Function attributes
    template <typename F>
    static R function_call(const void *p) {
        return (*(F*)p)();
    }

    template <typename F>
    static void function_move(void *d, const void *p) {
        new (d) F(*(F*)p);
    }

    template <typename F>
    static void function_dtor(void *p) {
        ((F*)p)->~F();
    }

    // Wrappers for functions with context
    template <typename O, typename M>
    struct method_context {
        M method;
        O *obj;

        method_context(O *obj, M method)
            : method(method), obj(obj) {}

        R operator()() const {
            return (obj->*method)();
        }
    };

    template <typename F, typename A>
    struct function_context {
        F func;
        A *arg;

        function_context(F func, A *arg)
            : func(func), arg(arg) {}

        R operator()() const {
            return func(arg);
        }
    };
};

template <typename R>
Callback<R()> callback(R (*func)() = 0) {
    return Callback<R()>(func);
}

template<typename T, typename U, typename R>
Callback<R()> callback(U *obj, R (T::*method)()) {
    return Callback<R()>(obj, method);
}

#endif
