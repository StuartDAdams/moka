#pragma once

#include <array>

namespace moka
{
    enum class key
    {
        none = 0,
        esc,
        enter,
        tab,
        space,
        backspace,
        up,
        down,
        left,
        right,
        insert,
        del,
        home,
        end,
        page_up,
        page_down,
        print,
        plus,
        minus,
        left_bracket,
        right_bracket,
        semicolon,
        quote,
        comma,
        period,
        slash,
        backslash,
        tilde,
        f1,
        f2,
        f3,
        f4,
        f5,
        f6,
        f7,
        f8,
        f9,
        f10,
        f11,
        f12,
        numpad_0,
        numpad_1,
        numpad_2,
        numpad_3,
        numpad_4,
        numpad_5,
        numpad_6,
        numpad_7,
        numpad_8,
        numpad_9,
        keypad_0,
        keypad_1,
        keypad_2,
        keypad_3,
        keypad_4,
        keypad_5,
        keypad_6,
        keypad_7,
        keypad_8,
        keypad_9,
        a,
        b,
        c,
        d,
        e,
        f,
        g,
        h,
        i,
        j,
        k,
        l,
        m,
        n,
        o,
        p,
        q,
        r,
        s,
        t,
        u,
        v,
        w,
        x,
        y,
        z,
    };

    /**
     * \brief This contains a snapshot of the keyboard state.
     */
    class keyboard_state
    {
        friend class application;

        std::array<bool, 86> keys_;

        void set_key_down(key key);

        void set_key_up(key key);

    public:
        /**
         * \brief Check if a key is down.
         * \param key The key you want to check.
         * \return True if the key is down, otherwise false.
         */
        bool is_key_down(key key) const;

        /**
         * \brief Check if a key is up.
         * \param key The key you want to check.
         * \return True if the key is up, otherwise false.
         */
        bool is_key_up(key key) const;
    };

    /**
     * \brief The keyboard object. Updated in the main event loop so that it contains an up-to-date snapshot of the keyboard state.
     */
    class keyboard
    {
        friend class application;

        keyboard_state state_{};

    public:
        /**
         * \brief Get the latest keyboard state.
         * \return The latest keyboard state.
         */
        const keyboard_state& get_state() const;
    };
} // namespace moka