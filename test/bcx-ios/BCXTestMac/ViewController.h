//
//  ViewController.h
//  BCXTestMac
//
//  Created by hugo on 2019/12/27.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController<NSTableViewDataSource, NSTableViewDelegate>

@property (weak) IBOutlet NSTextField *txtLog;
@property (weak) IBOutlet NSTableView *testCasesView;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView;
- (nullable NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(nullable NSTableColumn *)tableColumn row:(NSInteger)row;
- (void)tableViewDoubleClick:(id)sender;

@end

