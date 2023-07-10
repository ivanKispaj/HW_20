
namespace chat
{
    enum Results
    {
        login_success,
        login_cancel,
        register_success,
        register_cancel,
        yes,
        no,
        app_exit,
        back,
        empty,
        chat,
        admin,
        registration,
        login,
        send_message,
        chat_options,
        private_chat,
        public_chat,
        search_user_byId,
        search_user_byName,
        search_user_byLogin,
        user_list,
        user_not_found,
        user_banned,
        user_is_service,
        user_profile,
        complaint,
        messages,
        delete_message,
        user_ban,
        user_unban,
        user_admin,
        user_not_admin,
        end,
    };
};
namespace user
{
    enum options
    {
        login,
        pass,
        name,
        end
    };
};
namespace page
{
    enum PaginationMode
    {
        page,
        last_page,
        message,
        msg_per_page,
        close_options,
    };
};