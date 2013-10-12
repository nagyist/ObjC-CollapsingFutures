#import <SenTestingKit/SenTestingKit.h>
#import "TOCFutureArrayUtil.h"
#import "TestUtil.h"

@interface TOCFutureArrayUtilTest : SenTestCase
@end

@implementation TOCFutureArrayUtilTest

-(void)testOrderedByCompletion {
    test([[@[] orderedByCompletion] isEqual:@[]]);
    testThrows([(@[@1]) orderedByCompletion]);
    
    NSArray* s = (@[[TOCFutureSource new], [TOCFutureSource new], [TOCFutureSource new]]);
    NSArray* f = (@[[s[0] future], [s[1] future], [s[2] future]]);
    NSArray* g = [f orderedByCompletion];
    test([g count] == [f count]);
    
    test([g[0] isIncomplete]);
    
    [s[1] trySetResult:@"A"];
    testFutureHasResult(g[0], @"A");
    test([g[1] isIncomplete]);
    
    [s[2] trySetFailure:@"B"];
    testFutureHasFailure(g[1], @"B");
    test([g[2] isIncomplete]);
    
    [s[0] trySetResult:@"C"];
    testFutureHasResult(g[2], @"C");
    
    // ordered by continuations, so after completion should preserve ordering of original array
    NSArray* g2 = [f orderedByCompletion];
    testFutureHasResult(g2[0], @"C");
    testFutureHasResult(g2[1], @"A");
    testFutureHasFailure(g2[2], @"B");
}
-(void) testFinallyAll {
    testThrows([@[@1] finallyAll]);
    test([[[@[] finallyAll] forceGetResult] isEqual:@[]]);
    
    TOCFuture* f = [(@[fut(@1), futfail(@2)]) finallyAll];
    test([f hasResult]);
    NSArray* x = [f forceGetResult];
    test([x count] == 2);
    testFutureHasResult(x[0], @1);
    testFutureHasFailure(x[1], @2);
}
-(void) testFinallyAll_Incomplete {
    TOCFutureSource* s = [TOCFutureSource new];
    TOCFuture* f = [(@[fut(@1), s.future, fut(@3)]) finallyAll];
    test([f isIncomplete]);
    
    [s trySetFailure:@""];
    test([f hasResult]);
    NSArray* x = [f forceGetResult];
    test([x count] == 3);
    testFutureHasResult(x[0], @1);
    testFutureHasFailure(x[1], @"");
    testFutureHasResult(x[2], @3);
}
-(void) testThenAll {
    testThrows([@[@1] thenAll]);
    test([[[@[] thenAll] forceGetResult] isEqual:@[]]);
    test([[[(@[fut(@3)]) thenAll] forceGetResult] isEqual:(@[@3])]);
    test([[[(@[fut(@1), fut(@2)]) thenAll] forceGetResult] isEqual:(@[@1, @2])]);
    
    TOCFuture* f = [(@[fut(@1), futfail(@2)]) thenAll];
    test([f hasFailed]);
    NSArray* x = [f forceGetFailure];
    test([x count] == 2);
    testFutureHasResult(x[0], @1);
    testFutureHasFailure(x[1], @2);
}
-(void) testThenAll_Incomplete {
    TOCFutureSource* s = [TOCFutureSource new];
    TOCFuture* f = [(@[fut(@1), s.future, fut(@3)]) thenAll];
    test([f isIncomplete]);
    [s trySetResult:@""];
    testFutureHasResult(f, (@[@1, @"", @3]));
}

@end
