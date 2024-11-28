#pragma once
#include <functional>

/*
* @brief 복사 생성자, 복사 대입 연산자를 삭제합니다.
*/
#define FZ_DELETE_COPY(ClassName)                       \
    ClassName(const ClassName&) = delete;               \
    ClassName& operator=(const ClassName&) = delete;
/*
* @brief 이벤트 콜백 멤버 함수를 등록합니다.
*/
#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

