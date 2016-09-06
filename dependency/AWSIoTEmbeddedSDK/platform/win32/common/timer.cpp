/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file timer.c
 * @brief Linux implementation of the timer interface.
 */

#include <vector>
#include <chrono>

#include "timer_platform.h"

bool has_timer_expired(Timer *timer) {
    std::chrono::system_clock::time_point temp_time = std::chrono::system_clock::from_time_t(timer->end_time);
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    auto diff_time = std::chrono::duration_cast<std::chrono::milliseconds>(temp_time - now);

    if (diff_time.count() <= 0)
        return true;

    return false;
}

void countdown_ms(Timer *timer, uint32_t timeout) {
    auto now = std::chrono::system_clock::now();

    now += std::chrono::milliseconds(timeout);

    timer->end_time = std::chrono::system_clock::to_time_t(now);
}

uint32_t left_ms(Timer *timer) {
    auto temp_time = std::chrono::system_clock::from_time_t(timer->end_time);
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    auto diff_time = std::chrono::duration_cast<std::chrono::milliseconds>(temp_time - now);

    temp_time -= diff_time;

    timer->end_time = std::chrono::system_clock::to_time_t(temp_time);
    
    if (diff_time.count() <= 0)
        return 0;

    return diff_time.count();
}

void countdown_sec(Timer *timer, uint32_t timeout) {
    auto now = std::chrono::system_clock::now();

    now += std::chrono::seconds(timeout);

    timer->end_time = std::chrono::system_clock::to_time_t(now);
}

void init_timer(Timer *timer) {
	timer->end_time = 0;
}
