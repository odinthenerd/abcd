#pragma once

namespace kvasir {
    namespace abcd {

		//index by type
        template<typename T>
        struct index_t {
        };

		//index by capability 
        template<typename T>
        struct capability_t {
		};

		template<template<typename> class F>
		struct wrap_trivial_public_interface {
			template<typename T>
			using f = F<T>;
		};

    }
}