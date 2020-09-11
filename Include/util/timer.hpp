
#pragma once

#include "../common/define.hpp"
#include "tool.hpp"
#include <map>

#define invalid_timer_id 0

using namespace std;

namespace shine
{
    typedef std::function<bool()> timer_callback_t;

    class timer
    {
    public:
        SHINE_GEN_MEMBER_GETSET(uint64, id);
        SHINE_GEN_MEMBER_GETSET(uint64, delay);
        SHINE_GEN_MEMBER_GETSET(uint64, timestamp);
		SHINE_GEN_MEMBER_GETSET(timer_callback_t, callback);
		SHINE_GEN_MEMBER_GETSET(bool, relative);
	};

    class timer_manager : std::multimap <uint64, timer>{
    public:
        timer_manager() {

        }

        /** 
         *@brief ִ�ж�ʱ��
         *@return shine::uint64 ���ض�ʱ�������Ƚ��������������ʱ��������λ����
         *@warning 
         *@note 
        */
        uint64 do_timer() {
            if (empty())
                return 0;
            uint64 now = tool::get_timestamp();

            while (size() > 0)
            {
                auto iter = begin();
                if (iter->first <= now) 
                {
                    timer item = iter->second;
                    erase(iter);
                    bool repeat = item.get_callback()();
                    if (repeat) {
						now = tool::get_timestamp();

						if (item.get_relative()) {
							item.set_timestamp(now + item.get_delay());
						}
						else {
							auto ts = item.get_timestamp() + item.get_delay();
							if (ts < now)
								ts = now;
							item.set_timestamp(ts);
						}
						insert(std::make_pair(item.get_timestamp(), item));
                    }

                }
                else {
                    break;
                }
            }

            return size() > 0 ? begin()->first - now : 0;
        }


        /** 
         *@brief ����һ����ʱ����
         *@param delay �ӳ�ʱ�䣬��λ����
		 *@param cb �ص�����
		 *@param relative �Ƿ�Ϊ���ʱ�䴥��
		 *@return shine::uint64 ���ض�ʱ����id
         *@warning 
         *@note 
        */
        uint64 set_timer(uint64 delay, timer_callback_t cb, bool relative = false) {
			if (delay == 0)
				return invalid_timer_id;

			timer item;
            item.set_id(++_id);
            item.set_delay(delay);
            item.set_timestamp(tool::get_timestamp() + delay);
            item.set_callback(std::move(cb));
			item.set_relative(relative);

            insert(std::make_pair(item.get_timestamp(), item));
            return item.get_id();
        }


        /** 
         *@brief ȡ��һ����ʱ����
         *@param id ����id
         *@return bool 
         *@warning 
         *@note 
        */
        bool cancel_timer(uint64 id) {
            if (id == invalid_timer_id)
                return false;

            for (auto iter = begin(); iter != end(); ++iter) {
                if (id == iter->second.get_id()) {
                    erase(iter);
                    return true;
                }
            }

            return false;
        }

		void clear_all() {
			clear();
		}
    private:
        SHINE_GEN_MEMBER_GETSET(uint64, id, = invalid_timer_id);
    };
}

