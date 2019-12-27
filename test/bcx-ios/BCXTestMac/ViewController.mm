//
//  ViewController.m
//  BCXTestMac
//
//  Created by hugo on 2019/12/27.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#import "ViewController.h"
#import "../../Classes/BCXTest.hpp"

static BCXTest BT;
static bool init = false;

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.

    if (!init) {
        init = true;
        BT.setNativeLog([self](const std::string& s) {
            [self showLog:[NSString stringWithUTF8String:s.c_str()]];
        });
        BT.init();
        [self testCasesViewSetting];
        [NSTimer scheduledTimerWithTimeInterval:0.1
                                        repeats:YES
                                          block:^(NSTimer * _Nonnull timer) {
            BT.loop();
        }];
    }
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (void)testCasesViewSetting {
    self.testCasesView.delegate = self;
    self.testCasesView.dataSource = self;
    self.testCasesView.target = self;
    self.testCasesView.doubleAction = @selector(tableViewDoubleClick:);
}

- (void)testCasePressed:(NSButton *)button {
    NSLog(@"Test:%@", button.title);
    [self showLog: [NSString stringWithFormat:@"Test:%@", button.title] ];
    BT.runTestCase(button.title.UTF8String);
}

- (void)showLog:(NSString*)s {
    if (nullptr == self.txtLog) {
        return;
    }
    NSString *txt = self.txtLog.stringValue;
    NSArray *sArr = [txt componentsSeparatedByString:@"\n"];
    NSMutableArray *mutArr = [NSMutableArray arrayWithArray:sArr];
    while (mutArr.count > 3) {
        [mutArr removeObjectAtIndex:0];
    }
    [mutArr addObject:s];
    txt = [mutArr componentsJoinedByString:@"\n"];
    self.txtLog.stringValue = txt;
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return BT.getTestCasesCount();
}

- (nullable NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(nullable NSTableColumn *)tableColumn row:(NSInteger)row {
    NSString *cellIdentifier = @"testcell";

    NSTableCellView *cell = [tableView makeViewWithIdentifier:cellIdentifier owner:self];
    if (cell) {
        cell.textField.stringValue = [NSString stringWithUTF8String:BT.getTestCasesName((int)row).c_str()];
        
        return cell;
    }
    return nil;
}

- (void)tableViewDoubleClick:(id)sender {
    NSInteger row = self.testCasesView.selectedRow;
    std::string testName = BT.getTestCasesName((int)row);
    NSLog(@"RunTest: %@", [NSString stringWithUTF8String: testName.c_str()]);
    BT.runTestCase(testName);
}

@end
