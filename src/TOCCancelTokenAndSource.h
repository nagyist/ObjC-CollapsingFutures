#import <Foundation/Foundation.h>

/*!
 * The type of block passed to TOCCancelToken's whenCancelled method.
 * The block is called when the token has been cancelled.
 */
typedef void (^TOCCancelHandler)(void);

/*!
 * Notifies you when operations should be cancelled.
 * 
 * @discussion A cancel token can be in three states: cancelled, can-be-cancelled, and immortal.
 *
 * The nil cancel token is considered to be immortal.
 *
 * A token in the immortal state is permanently immortal and not cancelled, and will immediately discard any cancel handlers being registered to it without running them.
 *
 * A token in the cancelled state is permanently cancelled, and will immediately run+discard any cancel handlers being registered to it.
 *
 * A token in the can-be-cancelled state can be cancelled by its source, causing it to run+discard all registered cancel handlers and transition to the cancelled state.
 *
 * A token in the can-be-cancelled state can also transition to the immortal state, if its source is deallocated, causing it to discard all registered cancel handlers without running them.
 *
 * TOCCancelToken is thread safe.
 * It can be accessed from multiple threads concurrently.
 *
 * Use whenCancelledDo to add a block to be called once the token has been cancelled.
 *
 * Use isAlreadyCancelled to determine if the token has already been cancelled, and canStillBeCancelled to determine if the token is not cancelled and not immortal.
 *
 * Use the TOCCancelTokenSource class to control your own TOCCancelToken instances.
 */
@interface TOCCancelToken : NSObject

/*!
 * Returns a token that has already been cancelled.
 *
 * @result A TOCCancelToken in the cancelled state.
 */
+(TOCCancelToken *)cancelledToken;

/*!
 * Returns a token that will never be cancelled.
 *
 * @result A TOCCancelToken permanently in the uncancelled state.
 *
 * @discussion Immortal tokens do not hold onto cancel handlers.
 * Cancel handlers given to an immortal token's whenCancelledDo will not be retained, stored, or called.
 *
 * This method is guaranteed to return a non-nil result, even though a nil cancel token is supposed to be treated exactly like an immortal token.
 */
+(TOCCancelToken *)immortalToken;

/*!
 * Determines if the token is in the cancelled state, as opposed to being can-be-cancelled or immortal.
 */
-(bool)isAlreadyCancelled;

/*!
 * Determines if the token is in the can-be-cancelled state, as opposed to being cancelled or immortal.
 */
-(bool)canStillBeCancelled;

/*!
 * Registers a cancel handler block to be called once the receiving token is cancelled.
 *
 * @param cancelHandler The block to call once the token is cancelled.
 *
 * @discussion If the token is already cancelled, the handler is run inline.
 *
 * If the token is or becomes immortal, the handler is not kept.
 *
 * The handler will be called either inline on the calling thread or on the thread that cancels the token.
 */
-(void)whenCancelledDo:(TOCCancelHandler)cancelHandler;

/*!
 * Registers a cancel handler block to be called once the receiving token is cancelled.
 *
 * @param cancelHandler The block to call once the token is cancelled.
 *
 * @param unlessCancelledToken If this token is cancelled before the receiving token, the handler is discarded without being called.
 *
 * @discussion If the token is already cancelled, the handler is run inline.
 *
 * If the token is or becomes immortal, the handler is not kept.
 *
 * If the same token is used as both the receiving and unlessCancelled token, the cancel handler is discarded without being run.
 *
 * The handler will be called either inline on the calling thread or on the thread that cancels the token.
 */
-(void) whenCancelledDo:(TOCCancelHandler)cancelHandler
                 unless:(TOCCancelToken*)unlessCancelledToken;

@end

/*!
 * Creates and controls a TOCCancelToken.
 *
 * @discussion Use the token property to access the token controlled by a token source.
 *
 * Use the cancel or tryCancel methods to cancel the token controlled by a token source.
 *
 * When a token source is deallocated, without its token having been cancelled, its token becomes immortal.
 * Immortal tokens discard all cancel handlers without running them.
 */
@interface TOCCancelTokenSource : NSObject

/*!
 * Returns the token controlled by the receiving token source.
 */
@property (readonly, nonatomic) TOCCancelToken* token;

/*!
 * Cancels the token controlled by the receiving token source.
 *
 * @discussion If the token has already been cancelled, cancel has no effect.
 */
-(void) cancel;

/*!
 * Attempts to cancel the token controlled by the receiving token source.
 *
 * @result True if the token controlled by this source transitioned to the cancelled state, or false if the token was already cancelled.
 */
-(bool) tryCancel;

@end
