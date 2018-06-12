#pragma once

#include <window/window.hpp>
#include <safe_queue.hpp>
#include <atomic>
#include <unordered_map>
#include <iostream>
#include <typeindex>

#undef main

namespace loki
{
    class base_event_handler
    {
    public:
        virtual ~base_event_handler() = default;
        virtual event_category category() = 0;
        virtual void notify(event* e) = 0;
    };

    template<typename concrete_event>
    class event_handler : public base_event_handler
    {
        void notify(event* e) override
        {
            if (e->category == category())
            {
                on_notify(*static_cast<concrete_event*>(e));
            }
        }

    public:
        event_category category() override
        {
            return concrete_event::category();
        }

        signal<concrete_event&> on_notify;
    };

    /**
     * \brief A subscriber is composed of multiple event handlers. Each event handler deals with a concrete type of event.
     * Being a child class of subscriber gives you access to the event system, and allows you to subscribe to events from the application-wide event loop.
     */
    class subscriber
    {
        std::unordered_map<event_category, std::shared_ptr<base_event_handler>> m_notifiers;

        static std::atomic<size_t> m_available_ids;
    public:

        void notify(event* e)
        {
            auto notifier = m_notifiers.at(e->category);
            notifier->notify(e);
        }

        subscriber()
        {
            m_id = ++m_available_ids;
        }

        template <typename T>
        signal_id connect(std::function<void(T&)>&& slot)
        {
            auto notifier = std::make_shared<event_handler<T>>();
            m_notifiers.emplace(T::category(), notifier);
            return notifier->on_notify.connect(std::move(slot));
        }

        virtual ~subscriber() = default;

        size_t m_id;
    };

    class event_dispatcher
    {
        event_queue m_events;
        std::unordered_map<size_t, subscriber*> m_subscribers;
        std::thread m_thread;
        std::mutex m_mutex;
        std::condition_variable c;
        bool dispatch_events = true;
    public:
        static void try_notify(subscriber* s, event* e)
        {
            if(s)
            {
                s->notify(e);
            }
        }

        event_dispatcher()
        = default;

        ~event_dispatcher()
        {
            dispatch_events = false;
            m_thread.join();
        }

        void run()
        {
            m_thread = std::thread
            {
                [this]
                {
                    std::cout << "Starting message dispatcher thread: " << std::this_thread::get_id() << std::endl;

                    while (dispatch_events)
                    {
                        // blocking call until next event is enqueued
                        auto&& queue_item = m_events.dequeue();

                        // get event and subscriber objects
                        auto&& q_event = std::move(queue_item.event);
                        const auto& q_subscriber = queue_item.subscriber;

                        // does the event have a subscriber attatched to it?
                        if (q_subscriber)
                        {
                            // notify the specific subscriber
                            try_notify(q_subscriber, q_event.get());
                        }
                    }
                }
            };
        }

        bool registered(subscriber* recipient)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_subscribers.find(recipient->m_id) != m_subscribers.end();
        }

        void register_subscribers(subscriber* recipient)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_subscribers[recipient->m_id] = recipient;
        }

        void try_register(subscriber* recipient)
        {
            if (!registered(recipient))
            {
                register_subscribers(recipient);
            }
        }

        template<typename event_type, typename = std::enable_if<std::is_base_of_v<event, event_type>>>
        void post_event(event_type&& event, subscriber* recipient = nullptr)
        {
            if (!registered(recipient))
            {
                register_subscribers(recipient);
            }
            m_events.enqueue(event_queue_item{ std::make_unique<event_type>(std::forward<event_type>(event)), recipient });
        }
    };

    class application
    {
        event_dispatcher m_dispatcher;

        window m_window;

        bool running = true;
    public:
        template<typename T, typename event_type, typename = std::enable_if<std::is_base_of_v<event, event_type>>>
        void post_event(event_type&& event, subscriber* recipient)
        {
            m_dispatcher.post_event(std::move(event), recipient);
        }

        application(int argc, char* argv[]);
        ~application();
        int run();
    };

    class graphics_device : public subscriber
    {
    public:
        graphics_device()
        {
            connect<asset_loaded_event>([](asset_loaded_event& event)
            {
            });

            connect<window_event>([](window_event& event)
            {
            });
        }
    };
}