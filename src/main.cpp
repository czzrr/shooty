int main()
{
  game g;
  
  asio::io_context io_context;
  unsigned int port = 60000;
  server srv(io_context, port, g);
  std::thread t([&]() { io_context.run(); });
  
  srv.writeToAll(g);
  std::queue<owned_player_action>& incoming_msgs = srv.incoming();
  while(true)
    {
      // If any incoming messages, update game state according to them
      while (!incoming_msgs.empty())
        {
          owned_player_action opa = incoming_msgs.front();
          //std::cout << opa.get_id() << ":" << get_player_action_str(opa.get_action()) << "\n";
          if (!g.do_action(opa.get_id(), opa.get_action()))
            {
              std::cout << "player " << opa.get_id() << " not found\n";
              srv.disconnect_from_client(opa.get_id());
            }
          incoming_msgs.pop();
        }
      
      g.advance();
      srv.writeToAll(g);
      SDL_Delay(1000 / FRAMES_PER_SECOND);
    }

  
  return 0;
}
