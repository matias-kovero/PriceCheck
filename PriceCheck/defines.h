#pragma once

#define CVAR_PROVIDER "pc_provider"
#define CVAR_DATA_STATUS "pc_data_status"

#define CVAR_USE_AVG "pc_use_avg"
#define CVAR_GIVE_X "pc_trade_x"
#define CVAR_GIVE_Y "pc_trade_y"
#define CVAR_FORCE_SHOW "pc_force_show"

#define CVAR_RECV_X "pc_recv_x"
#define CVAR_RECV_Y "pc_recv_y"

#define CVAR_HEIGHT "pc_height"
#define CVAR_WIDTH "pc_width"

/* TRADE HOOKS */
#define HOOK_TRADE_START "Function TAGame.GFxData_TradeLobby_TA.HandleAcceptedInviteToTrade"
#define HOOK_TRADE_END "Function TAGame.GFxData_TradeLobby_TA.CancelTrading"
#define HOOK_TRADE_FORCE_END "Function TAGame.GFxData_TradeLobby_TA.CloseTradingWindow"
#define HOOK_TRADE_CHANGE "Function TAGame.GFxData_TradeLobby_TA.GetProductOfferings"

/* ITEM DROP HOOKS */
#define HOOK_NEW_ITEM "Function TAGame.GFxData_MultiItemDrops_TA.HandleNewOnlineItem" // When an item drops to us.
#define HOOK_PENDING_DROP "Function TAGame.GFxData_MultiItemDrops_TA.GetPendingDropCount" // When item drop is visible on screen
#define HOOK_DROPS_ENDED "Function TAGame.GFxData_MultiItemDrops_TA.PlayerAllDropsHandled" // Item drops have ended -> clean-up

/* TRADE-IN HOOKS */
#define HOOK_TRADE_IN_UPDATE "Function TAGame.GFxData_ProductTradeIn_TA.UpdateTradeInQuantity" // When items are changed in tradeIn
#define HOOK_TRADE_IN_DONE "Function TAGame.GFxData_ProductTradeIn_TA.HandleTradeInComplete"

/* CAPSULE OPENING */
#define HOOK_SHOW_NEW_ITEM "Function TAGame.PremiumGaragePreviewSet_TA.OnRevealFinished"