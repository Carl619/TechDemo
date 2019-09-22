#pragma once
#pragma once
#include <vector>
#include <map>
#include <stack>

namespace NCL {
	namespace CSC8503 {
		enum GameState {
			MAINMENU,LEVELMENU,GAME,RESULTMENU,EXIT,LOADING,LEVEL1,LEVEL2,LEVEL3,ONLINE,ONLINESERVER,ONLINECLIENT,ONLINERESULTCLIENT,ONLINERESULTSERVER, ONLINECHECKRESULTS, DISCONNECTCLIENT
		};
		class State;
		class StateTransition;

		typedef std::multimap<State*, StateTransition*> TransitionContainer;
		typedef TransitionContainer::iterator TransitionIterator;

		class GameStateMachine {
		public:
			static GameStateMachine * getInstance();
			void popState() { stackState.pop(); }
			GameState getActiveState() {
				return stackState.top();
			}
			void pushState(GameState state) { stackState.push(state); }
			void replace(GameState state) {
				stackState.pop();
				stackState.push(state); }
			void changeState(GameState state) { activeState = state; }
		protected:
			GameStateMachine();
			~GameStateMachine();
			std::stack<GameState> stackState;
			GameState activeState;
		};
	}
}